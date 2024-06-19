#include "Program.h"
#include "cpptoml.h"
#include "MaskColorCheckerGUI.h"

#include "stb_image.h" //first defined in Canvas.cpp

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <FL/Fl.H>
#include <FL/fl_ask.H> 
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Native_File_Chooser.H>

#include <string>
#include <filesystem>
#include <deque>
#include <mutex>
#include <thread>
#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>
#include <map>
#include <vector>

unsigned int  g_tileSize      = 256;
unsigned int  g_overlap       = 0;
unsigned int  g_tilesCount    = 0;
unsigned int  g_colorsPerTile = 5;
Fl_Color      g_badTileColor  = 0xFF000000;
Fl_Color      g_gridColor     = 0x00FFFF00;
bool          g_solidFill     = false;
bool          g_unsaved       = false;
bool          g_drawGrid      = false;

std::filesystem::path  g_maskPath;
std::filesystem::path  g_projectPath;

unsigned char* g_maskImg = nullptr;
int g_maskImgX = 0;
int g_maskImgY = 0;

std::deque<Tile> g_faultyTiles;
std::mutex g_faultyTilesQueueMutex;

std::deque<std::filesystem::path> g_recentProjects;

void PlotLine(unsigned int* img, int x0, int y0, int x1, int y1, unsigned int color)
{
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2;

	while(true)
	{
		if (y0 * g_maskImgY + x0 < g_maskImgX * g_maskImgY)
			img[y0 * g_maskImgY + x0] = color;
		if (x0 == x1 && y0 == y1) break;
		e2 = 2 * err;
		if (e2 >= dy) { err += dy; x0 += sx; }
		if (e2 <= dx) { err += dx; y0 += sy; }
	}
}

void FixFragment(unsigned char* image, Fragment& f, std::vector<unsigned int> colorsToReplace, unsigned int backupColor)
{
	// If the fragment is disabled (edge and corner tiles), return immediately
	if (f.w == 0) 
	{
		return;
	}
	unsigned int startX, startY, sizeX, sizeY, mostCommonColor;
	startX = f.x;
	startY = f.y;
	sizeX = f.w;
	sizeY = f.h;
	bool mostCommonColorSet = false;
	for (unsigned int y = startY; y < startY + sizeY; y++) 
	{
		for (unsigned int x = startX; x < startX + sizeX; x++)
		{
			unsigned int rgb = (image[(y * g_maskImgY * 3) + (x * 3)] << 16) |
				(image[(y * g_maskImgY * 3) + (x * 3) + 1] << 8) |
				image[(y * g_maskImgY * 3) + (x * 3) + 2];

			for (unsigned int colorToReplace : colorsToReplace)
			{
				if (rgb == colorToReplace)
				{
					if (!mostCommonColorSet)
					{
						// Get the most frequent color in this fragment
						// but only if there is actually need for it.
						std::unordered_map<uint32_t, int> colorCount;
						for (int y1 = startY; y1 < startY + sizeY; y1++)
						{
							for (int x1 = startX; x1 < startX + sizeX; x1++)
							{
								uint32_t rgb1 = (image[(y1 * g_maskImgY * 3) + (x1 * 3)] << 16) |
									(image[(y1 * g_maskImgY * 3) + (x1 * 3) + 1] << 8) |
									image[(y1 * g_maskImgY * 3) + (x1 * 3) + 2];
								colorCount[rgb1]++;
							}
						}

						std::vector<std::pair<uint32_t, int>> colors;

						for (auto& pair : colorCount)
						{
							colors.push_back(pair);
						}

						std::sort(colors.begin(), colors.end(), [](const auto& a, const auto& b)
						{
							return a.second > b.second;
						});

						// Now find the most common color in this fragment
						// that isn't on the list of color to be replaced.
						// If that's not possible, use the backup color
						for (unsigned int i = 0; i < colors.size(); i++)
						{
							bool isToBeReplaced = false;
							for (unsigned int c : colorsToReplace)
							{
								if (c == colors[i].first)
								{
									isToBeReplaced = true;
									break;
								}
							}
							if (!isToBeReplaced)
							{
								mostCommonColor = colors[i].first;
								mostCommonColorSet = true;
								break;
							}
						}
						if (!mostCommonColorSet)
						{
							//fl_message_title("Manual intervention needed");
							//std::string at = "At X = " + std::to_string(x) + " y = " + std::to_string(y);
							//fl_alert(at.c_str());
							//mostCommonColor = g_badTileColor >> 8;
							mostCommonColorSet = true;
							mostCommonColor = backupColor;
						}
						//mostCommonColor = colors.back().first;
					}
					image[(y * g_maskImgY * 3) + (x * 3)] = (mostCommonColor >> 16) & 0xFF;
					image[(y * g_maskImgY * 3) + (x * 3) + 1] = (mostCommonColor >> 8) & 0xFF;
					image[(y * g_maskImgY * 3) + (x * 3) + 2] = mostCommonColor & 0xFF;
				}
			}
		}
	}
}

void FixTile(unsigned char* image, Tile& t)
{
	// First, get the color counts for the entire tile, and sort them by rarity.
	std::unordered_map<uint32_t, unsigned int> colorCount;
	for (unsigned int y = t.startY; y < t.startY + t.sizeY; y++)
	{
		for (unsigned int x = t.startX; x < t.startX + t.sizeX; x++)
		{
			uint32_t rgb = (image[(y * g_maskImgY * 3) + (x * 3)] << 16) |
				(image[(y * g_maskImgY * 3) + (x * 3) + 1] << 8) |
				image[(y * g_maskImgY * 3) + (x * 3) + 2];
			colorCount[rgb]++;
		}
	}
	// Fixing one of the previous tiles may have fixed this one too.
	// If that happens we can skip this tile
	if (colorCount.size() <= g_colorsPerTile)
		return;

	std::vector<std::pair<uint32_t, unsigned int>> colors;
	for (auto& pair : colorCount)
	{
		colors.push_back(pair);
	}

	std::sort(colors.begin(), colors.end(), [](const auto& a, const auto& b)
	{
		return a.second < b.second;
	});

	// Now get the N rarest colors that need to be replaced
	std::vector<unsigned int> colorsToReplace;
	for (unsigned int i = 0; i < colorCount.size() - g_colorsPerTile; i++)
	{
		colorsToReplace.push_back(colors[i].first);
	}

	// Check all the fragments
	FixFragment(image, t.NW, colorsToReplace, colors.back().first);
	FixFragment(image, t.N, colorsToReplace, colors.back().first);
	FixFragment(image, t.NE, colorsToReplace, colors.back().first);
	FixFragment(image, t.W, colorsToReplace, colors.back().first);
	FixFragment(image, t.M, colorsToReplace, colors.back().first);
	FixFragment(image, t.E, colorsToReplace, colors.back().first);
	FixFragment(image, t.SW, colorsToReplace, colors.back().first);
	FixFragment(image, t.S, colorsToReplace, colors.back().first);
	FixFragment(image, t.SE, colorsToReplace, colors.back().first);
}

void AddFaultyTile(unsigned int x, unsigned int y, unsigned int px, unsigned int py, unsigned int sx, unsigned int sy)
{
	std::lock_guard<std::mutex> lock(g_faultyTilesQueueMutex);
	g_faultyTiles.emplace_back(Tile(x, y, px, py, sx, sy));
}

void CheckColumn(unsigned int x)
{
	int spacing = g_tileSize - g_overlap;
	int xPos = (spacing * x) - (g_overlap / 2.f);
	int tileWidth = g_tileSize;

	if (x == 0)
	{
		tileWidth = g_tileSize - (g_overlap / 2.f);
		xPos = 0;
	}
	else if (x == g_tilesCount - 1)
	{
		tileWidth = g_maskImgX - xPos - 1;
	}

	for (unsigned int y = 0; y < g_tilesCount; y++)
	{
		int tileHeight = g_tileSize;
		int yPos = (spacing * y) - (g_overlap / 2.f);
		if (y == 0)
		{
			tileHeight = g_tileSize - (g_overlap / 2.f);
			yPos = 0;
		}
		else if (y == g_tilesCount - 1)
		{
			tileHeight = g_maskImgY - yPos - 1;
		}

		std::deque<unsigned int> tileColors;

		for (unsigned int px = xPos; px < tileWidth + xPos; px++)
		{
			for (unsigned int py = yPos; py < tileHeight + yPos; py++)
			{
				unsigned char r, g, b;
				r = g_maskImg[(3 * g_maskImgY * py) + (3 * px)];
				g = g_maskImg[(3 * g_maskImgY * py) + (3 * px) + 1];
				b = g_maskImg[(3 * g_maskImgY * py) + (3 * px) + 2];
				unsigned int color = r << 16 | g << 8 | b;
				if (!tileColors.empty() && std::find(tileColors.begin(), tileColors.end(), color) != tileColors.end())
				{
					continue;
				}
				tileColors.emplace_back(color);
				if (tileColors.size() > g_colorsPerTile)
				{
					AddFaultyTile(x, y, xPos, yPos, tileWidth, tileHeight);
					goto skipRestOfTile;
				}
			}
		}
	skipRestOfTile:
		continue;
	}
}

void CheckColumns(unsigned int firstX, unsigned int lastX)
{
	if (firstX == 1) firstX = 0;
	for (unsigned int x = firstX; x <= lastX; x++)
	{
		CheckColumn(x);
	}
}

bool CheckParametersValid()
{
	int spacing = g_tileSize - g_overlap;
	int lastTileStart = (spacing * (g_tilesCount - 1)) - (g_overlap / 2.f);
	int secondToLastTileStart = (spacing * (g_tilesCount - 2)) - (g_overlap / 2.f);
	if ((g_tileSize != 128 && g_tileSize != 256 && g_tileSize != 512 && g_tileSize != 1024 && g_tileSize != 2048) ||
		(g_colorsPerTile < 4 || g_colorsPerTile > 6) ||
		(g_maskImg == nullptr) || lastTileStart > g_maskImgX || lastTileStart < g_maskImgX - g_tileSize ||
		secondToLastTileStart >= g_maskImgX - g_tileSize)
	{
		g_parametersStatusBox->label("Invalid parameters");
		g_parametersStatusBox->activate();
		g_parametersStatusBox->redraw();
		g_menuItemSave->deactivate();
		g_menuItemSaveAs->deactivate();
		g_checkButton->deactivate();
		g_saveGridButton->deactivate();
		return false;
	}
	g_parametersStatusBox->label("Valid parameters");
	g_parametersStatusBox->deactivate();
	g_parametersStatusBox->redraw();
	g_menuItemSave->activate();
	g_menuItemSaveAs->activate();
	g_checkButton->activate();
	g_saveGridButton->activate();
	return true;
}

void MenuCB(Fl_Widget* widget, void* d)
{
	int data = (int)d;
	if (data == CB_QUIT)
	{
		exit(0);
	}
	if (data >= 200) // Load recent project
	{
		g_faultyTiles.clear();
		std::filesystem::path toOpen =g_recentProjects[data - 200];
		toOpen.replace_extension(".toml");
		std::shared_ptr<cpptoml::table> root;
		try
		{
			root = cpptoml::parse_file(toOpen.u8string());
		}
		catch (const cpptoml::parse_exception& e)
		{
			fl_message_title("Error");
			fl_alert("Failed to load project file");
			return;
		}

		std::shared_ptr<cpptoml::table> mcc = root->get_table("maskcolorcheckerproject");
		if (mcc == nullptr)
		{
			fl_message_title("Error");
			fl_alert("Failed to load project file");
			return;
		}

		g_tileSize = mcc->get_as<unsigned int>("tileSize").value_or(256);
		g_overlap = mcc->get_as<unsigned int>("actualOverlap").value_or(0);
		g_tilesCount = mcc->get_as<unsigned int>("tilesInRow").value_or(0);
		g_colorsPerTile = mcc->get_as<unsigned int>("colorsPerTile").value_or(0);
		g_badTileColor = mcc->get_as<unsigned int>("badTileColor").value_or(16777216);
		g_gridColor = mcc->get_as<unsigned int>("gridColor").value_or(16777216);

		g_maskPath = std::filesystem::u8path(mcc->get_as<std::string>("maskPath").value_or(""));

		if (g_maskPath.empty() &&
			(g_tileSize != 128 && g_tileSize != 256 && g_tileSize != 512 && g_tileSize != 1024 && g_tileSize != 2048) &&
			(g_overlap == 0 || g_tilesCount == 0 || g_colorsPerTile == 0 || g_badTileColor == 16777216 || g_gridColor == 16777216))
			if (mcc == nullptr)
			{
				fl_message_title("Error");
				fl_alert("Failed to load project file");
				return;
			}

		switch (g_tileSize)
		{
		case 128:
			g_tileSizeChoice->value(0);
			break;
		case 256:
			g_tileSizeChoice->value(1);
			break;
		case 512:
			g_tileSizeChoice->value(2);
			break;
		case 1024:
			g_tileSizeChoice->value(3);
			break;
		case 2048:
			g_tileSizeChoice->value(4);
			break;
		}

		g_overlapSpinner->value(g_overlap);
		g_tilesInRowSpinner->value(g_tilesCount);
		g_colorsPerTileSpinner->value(g_colorsPerTile);
		g_badTileFillColorButton->color(g_badTileColor);
		g_gridColorButton->color(g_gridColor);

		int n;
		if (g_maskImg != nullptr)
			stbi_image_free(g_maskImg);
		g_maskImg = stbi_load(g_maskPath.u8string().c_str(), &g_maskImgX, &g_maskImgY, &n, 3);
		if (g_maskImg == nullptr)
		{
			fl_message_title("Error");
			fl_alert("Failed to load project file");
			return;
		}
		g_canvas->set(g_maskImg, g_maskImgX, g_maskImgY, &g_faultyTiles);
		g_maskPathOutput->value(g_maskPath.u8string().c_str());
		CheckParametersValid();
		g_projectPath = toOpen;
	}
}

void InitializeProgram()
{

#ifdef _WIN32
	std::string appdata = std::getenv("APPDATA");
	std::filesystem::path configPath = std::filesystem::path(appdata) / "MaskColorChecker" / "MaskColorChecker.toml";
#else
	std::string home = std::getenv("HOME");
	std::filesystem::path configPath = std::filesystem::path(home) / ".config" / "MaskColorChecker" / "MaskColorChecker.toml";
#endif
	if(std::filesystem::exists(configPath))
	{
		std::shared_ptr<cpptoml::table> root = cpptoml::parse_file(configPath.u8string());
		if (root != nullptr)
		{
			std::shared_ptr<cpptoml::table> config = root->get_table("maskcolorchecker");
			if (config != nullptr)
			{
				std::shared_ptr<cpptoml::array> recentProjects = config->get_array("recent");
				if (recentProjects != nullptr)
				{
					unsigned int i = 0;
					for (const auto& val : *recentProjects)
					{
						std::filesystem::path path = std::filesystem::path(val->as<std::string>()->get());
						g_recentProjects.push_back(path);
						std::string entry = "Project/_@reload Recent/    " + path.stem().u8string();
						g_menuBar->add(entry.c_str(), 0, (Fl_Callback*)MenuCB, (void*)(g_recentProjects.size() - 1 + 200), 0);
						i++;
					}
				}
			}
		}
	}

	g_menuBar->add("Project/@1+ Quit", 'q' + FL_CTRL, (Fl_Callback*)MenuCB, (void*)CB_QUIT, 0);
	CheckParametersValid();
}

void ShutdownProgram()
{
	if (g_maskImg != nullptr)
		stbi_image_free(g_maskImg);
#ifdef _WIN32
	std::string appdata = std::getenv("APPDATA");
	std::filesystem::path configPath = std::filesystem::path(appdata) / "MaskColorChecker" / "MaskColorChecker.toml";
#else
	std::string home = std::getenv("HOME");
	std::filesystem::path configPath = std::filesystem::path(home) / ".config" / "MaskColorChecker" / "MaskColorChecker.toml";
#endif
	if (!std::filesystem::exists(configPath.parent_path()))
	{
		std::filesystem::create_directory(configPath.parent_path());
	}
	std::ofstream configFile(configPath);
	if (configFile.good())
	{
		std::shared_ptr<cpptoml::table> root = cpptoml::make_table();
		std::shared_ptr<cpptoml::table> config = cpptoml::make_table();
		std::shared_ptr<cpptoml::array> recentProjects = cpptoml::make_array();

		for (std::filesystem::path& path : g_recentProjects)
		{
			recentProjects->push_back(path.u8string().c_str());
		
		}
		config->insert("recent", recentProjects);
		root->insert("maskcolorchecker", config);

		configFile << *root;

		configFile.close();
	}
}

void OnGui(int e)
{
	if (e == CB_QUIT)
	{
		exit(0);
	}
	else if (e == CB_ABOUT)
	{
		fl_message_title("About");
		fl_message("Mask Color Checker\nWaldemar Karpowicz 2024\n\nMask Color Checker is based in part on the works of the following projects:\nFLTK (http://www.fltk.org)\nSTB (https://github.com/nothings/stb)\ncpptoml (https://github.com/skystrife/cpptoml)\n\nThanks to Pennyworth et al. for documenting the mapframe properties at\nhttps://github.com/pennyworth12345/A3_MMSI/wiki/Mapframe-Information");
	}
	else if (e == CB_TILESIZE)
	{
		unsigned int val = (unsigned int)g_tileSizeChoice->value();
		switch (val)
		{
		default:
		case 0:
			g_tileSize = 128;
			break;
		case 1:
			g_tileSize = 256;
			break;
		case 2:
			g_tileSize = 512;
			break;
		case 3:
			g_tileSize = 1024;
			break;
		case 4:
			g_tileSize = 2048;
			break;
		}
		g_unsaved = true;
		g_menuItemSave->activate();
		g_faultyTiles.clear();
		CheckParametersValid();
	}
	else if (e == CB_OVERLAP)
	{
		g_overlap = (unsigned int)g_overlapSpinner->value();
		g_unsaved = true;
		g_menuItemSave->activate();
		g_faultyTiles.clear();
		CheckParametersValid();
	}
	else if (e == CB_TILECOUNT)
	{
		g_tilesCount = (unsigned int)g_tilesInRowSpinner->value();
		g_unsaved = true;
		g_menuItemSave->activate();
		g_faultyTiles.clear();
		CheckParametersValid();
	}
	else if (e == CB_COLORSPERTILE)
	{
		g_colorsPerTile = (unsigned int)g_colorsPerTileSpinner->value();
		g_unsaved = true;
		g_menuItemSave->activate();
		CheckParametersValid();
	}
	else if (e == CB_BADTILECOLOR)
	{
		unsigned char r, g, b;
		r = g_badTileColor >> 24;
		g = g_badTileColor >> 16;
		b = g_badTileColor >> 8;
		fl_color_chooser("Select color", r, g, b, 0);
		if(g_badTileColor != fl_rgb_color(r, g, b))
		{
			g_badTileColor = fl_rgb_color(r, g, b);
			g_badTileFillColorButton->color(g_badTileColor);
			g_badTileFillColorButton->redraw();
			g_unsaved = true;
			g_menuItemSave->activate();
			g_canvas->redraw();
		}
	}
	else if (e == CB_GRIDCOLOR)
	{
		unsigned char r, g, b;
		r = g_gridColor >> 24;
		g = g_gridColor >> 16;
		b = g_gridColor >> 8;
		fl_color_chooser("Select color", r, g, b, 0);
		if (g_gridColor != fl_rgb_color(r, g, b))
		{
			g_gridColor = fl_rgb_color(r, g, b);
			g_gridColorButton->color(g_gridColor);
			g_gridColorButton->redraw();
			g_unsaved = true;
			g_menuItemSave->activate();
			g_canvas->redraw();
		}
	}
	else if (e == CB_DRAWGRID)
	{
		g_drawGrid = !g_drawGrid;
		g_unsaved = true;
		g_menuItemSave->activate();
		g_canvas->redraw();
	}
	else if (e == CB_SOLIDFILL)
	{
		g_solidFill = !g_solidFill;
		g_unsaved = true;
		g_menuItemSave->activate();
		g_canvas->redraw();
	}
	else if (e == CB_LOADMASK)
	{
		Fl_Native_File_Chooser browseMaskFileChooser;
		browseMaskFileChooser.title("Pick a mask file");
		browseMaskFileChooser.filter("24bit BMP\t*.bmp");
		browseMaskFileChooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
		if (browseMaskFileChooser.show() != 1)
		{
			g_faultyTiles.clear();
			g_maskPath = std::filesystem::u8path(browseMaskFileChooser.filename());
			int n;
			if (g_maskImg != nullptr)
				stbi_image_free(g_maskImg);
			g_maskImg = stbi_load(browseMaskFileChooser.filename(), &g_maskImgX, &g_maskImgY, &n, 3);
			if (g_maskImg == nullptr)
			{
				fl_message_title("Error");
				fl_alert("Failed to load the mask");
				return;
			}
			g_canvas->set(g_maskImg, g_maskImgX, g_maskImgY, &g_faultyTiles);
			g_maskPathOutput->value(browseMaskFileChooser.filename());
			g_unsaved = true;
			g_menuItemSave->activate();
			CheckParametersValid();
			g_canvas->redraw();
			g_fixButton->deactivate();
			g_saveOutputButton->deactivate();
		}

	}
	else if (e == CB_CHECK)
	{
		g_faultyTiles.clear();
		std::vector<std::thread> threads(8);

		int columnsPerThread = g_tilesCount / 8;
		int remainingColumns = g_tilesCount % 8;

		for (int i = 0; i < 8; i++) 
		{
			int start = i * columnsPerThread + 1;
			int end = (i + 1) * columnsPerThread;
			if (i == 7) {
				end += remainingColumns - 1;
			}
			threads[i] = std::thread(CheckColumns, start, end);
		}
		for (auto& thread : threads) 
		{
			thread.join();
		}

		if (g_faultyTiles.size() > 0)
		{
			fl_message_title("Problems found");
			std::string msg = "Found " + std::to_string(g_faultyTiles.size()) + " problematic tile" + (g_faultyTiles.size() > 1 ? "s." : ".");
			fl_alert(msg.c_str());
			g_fixButton->activate();
			g_saveOutputButton->activate();
			return;
		}
		fl_message_title("No problems found");
		std::string msg = "Found zero problematic tiles.";
		fl_alert(msg.c_str());
		g_canvas->redraw();
		g_fixButton->deactivate();
		g_saveOutputButton->deactivate();
	}
	else if (e == CB_FIX)
	{
		Fl_Native_File_Chooser fixFileChooser;
		fixFileChooser.title("Save the fixed mask");
		fixFileChooser.filter("24bit BMP\t*.bmp");
		fixFileChooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
		if (fixFileChooser.show() == 0)
		{
			unsigned char* fixedMask = new unsigned char[g_maskImgX * g_maskImgY * 3];
			std::memcpy(fixedMask, g_maskImg, g_maskImgX* g_maskImgY * 3);
			for (Tile& t : g_faultyTiles)
			{
				FixTile(fixedMask, t);
			}
			std::filesystem::path toSaveTo = std::filesystem::u8path(fixFileChooser.filename());
			toSaveTo.replace_extension(".bmp");
			stbi_write_bmp(toSaveTo.u8string().c_str(), g_maskImgX, g_maskImgY, 3, fixedMask);
			delete[] fixedMask;
		}
	}
	else if (e == CB_SAVEOUTPUT)
	{
		Fl_Native_File_Chooser badTilesFileChooser;
		badTilesFileChooser.title("Save bad tiles mask");
		badTilesFileChooser.filter("32bit BMP\t*.bmp");
		badTilesFileChooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
		if (badTilesFileChooser.show() == 0)
		{
			unsigned int* badTiles = new unsigned int[g_maskImgX * g_maskImgY];
			std::memset(badTiles, 0, g_maskImgX * g_maskImgY * 4);
			// For some retarded reason, 32bit BMP needs ABGR.
			unsigned int badTileColor = 0xFF00FFFF;
			for (Tile& t : g_faultyTiles)
			{
				if(g_solidFill)
				{
					for (unsigned int x = t.startX; x < t.startX + t.sizeX; x++)
					{
						for (unsigned int y = t.startY; y < t.startY + t.sizeY; y++)
						{
							badTiles[y * g_maskImgY + x] = badTileColor;
						}
					}
				}
				else
				{
					PlotLine(badTiles, t.startX, t.startY, t.startX + t.sizeX, t.startY, badTileColor);
					PlotLine(badTiles, t.startX, t.startY + t.sizeY, t.startX + t.sizeX, t.startY + t.sizeY, badTileColor);
					PlotLine(badTiles, t.startX, t.startY, t.startX, t.startY + t.sizeY, badTileColor);
					PlotLine(badTiles, t.startX + t.sizeX, t.startY, t.startX + t.sizeX, t.startY + t.sizeY, badTileColor);
				}
			}
			std::filesystem::path toSaveTo = std::filesystem::u8path(badTilesFileChooser.filename());
			toSaveTo.replace_extension(".bmp");
			stbi_write_bmp(toSaveTo.u8string().c_str(), g_maskImgX, g_maskImgY, 4, badTiles);
			delete[] badTiles;
		}
	}
	else if (e == CB_SAVEGRID)
	{
		Fl_Native_File_Chooser gridFileChooser;
		gridFileChooser.title("Save grid");
		gridFileChooser.filter("32bit BMP\t*.bmp");
		gridFileChooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
		if (gridFileChooser.show() == 0)
		{
			unsigned int* grid = new unsigned int[g_maskImgX * g_maskImgY];
			std::memset(grid, 0, g_maskImgX * g_maskImgY * 4);
			unsigned int gridColor = 0xFFFF0000;
			int spacing = g_tileSize - g_overlap;
			for (unsigned int x = 0; x < g_tilesCount; x++)
			{
				int tileWidth = g_tileSize;
				int xPos = (spacing * x) - (g_overlap / 2.f);
				if (x == 0)
				{
					tileWidth = g_tileSize - (g_overlap / 2.f);
					xPos = 0;
				}
				else if (x == g_tilesCount - 1)
				{
					tileWidth = g_maskImgX - xPos + 1;
				}

				for (unsigned int y = 0; y < g_tilesCount; y++)
				{
					int tileHeight = g_tileSize;

					int yPos = (spacing * y) - (g_overlap / 2.f);
					if (y == 0)
					{
						tileHeight = g_tileSize - (g_overlap / 2.f);
						yPos = 0;
					}
					else if (y == g_tilesCount - 1)
					{
						tileHeight = g_maskImgY - yPos + 1;
					}
					PlotLine(grid, xPos, yPos, xPos + tileWidth, yPos, + gridColor);
					PlotLine(grid, xPos, yPos, xPos, yPos + tileHeight, gridColor);
					PlotLine(grid, xPos + tileWidth, yPos, xPos + tileWidth, yPos + tileHeight, gridColor);
					PlotLine(grid, xPos, yPos + tileHeight, xPos + tileWidth, yPos + tileHeight, gridColor);
				}
			}
			std::filesystem::path toSaveTo = std::filesystem::u8path(gridFileChooser.filename());
			toSaveTo.replace_extension(".bmp");
			stbi_write_bmp(toSaveTo.u8string().c_str(), g_maskImgX, g_maskImgY, 4, grid);
			delete[] grid;
		}
	}
	else if (e == CB_SAVE)
	{
		if (!g_projectPath.empty())
		{
			std::ofstream file(g_projectPath);
			if (file.good())
			{
				std::shared_ptr<cpptoml::table> root = cpptoml::make_table();
				std::shared_ptr<cpptoml::table> mcc = cpptoml::make_table();
				mcc->insert("tileSize", g_tileSize);
				mcc->insert("actualOverlap", g_overlap);
				mcc->insert("tilesInRow", g_tilesCount);
				mcc->insert("colorsPerTile", g_colorsPerTile);
				mcc->insert("badTileColor", g_badTileColor);
				mcc->insert("gridColor", g_gridColor);
				mcc->insert("maskPath", g_maskPath.generic_u8string());
				root->insert("maskcolorcheckerproject", mcc);
				file << *root;
				file.close();
			}
			g_unsaved = false;
			g_menuItemSave->deactivate();
			return;
		}
		Fl_Native_File_Chooser saveFileChooser;
		saveFileChooser.title("Save project file");
		saveFileChooser.filter("TOML file\t*.toml");
		saveFileChooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
		if (saveFileChooser.show() == 0)
		{
			std::filesystem::path toSaveTo = std::filesystem::u8path(saveFileChooser.filename());
			toSaveTo.replace_extension("toml");
			std::ofstream file(toSaveTo);
			if (file.good())
			{
				std::shared_ptr<cpptoml::table> root = cpptoml::make_table();
				std::shared_ptr<cpptoml::table> mcc = cpptoml::make_table();
				mcc->insert("tileSize", g_tileSize);
				mcc->insert("actualOverlap", g_overlap);
				mcc->insert("tilesInRow", g_tilesCount);
				mcc->insert("colorsPerTile", g_colorsPerTile);
				mcc->insert("badTileColor", g_badTileColor);
				mcc->insert("gridColor", g_gridColor);
				mcc->insert("maskPath", g_maskPath.generic_u8string());
				root->insert("maskcolorcheckerproject", mcc);
				file << *root;
				file.close();
				g_recentProjects.push_back(toSaveTo);
			}
			g_unsaved = false;
			g_menuItemSave->deactivate();
		}
	}
	else if (e == CB_SAVEAS)
	{
		Fl_Native_File_Chooser saveFileChooser;
		saveFileChooser.title("Save project file");
		saveFileChooser.filter("TOML file\t*.toml");
		saveFileChooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
		if (saveFileChooser.show() == 0)
		{
			std::filesystem::path toSaveTo = std::filesystem::u8path(saveFileChooser.filename());
			toSaveTo.replace_extension(".toml");
			std::ofstream file(toSaveTo);
			if (file.good())
			{
				std::shared_ptr<cpptoml::table> root = cpptoml::make_table();
				std::shared_ptr<cpptoml::table> mcc = cpptoml::make_table();
				mcc->insert("tileSize", g_tileSize);
				mcc->insert("actualOverlap", g_overlap);
				mcc->insert("tilesInRow", g_tilesCount);
				mcc->insert("colorsPerTile", g_colorsPerTile);
				mcc->insert("badTileColor", g_badTileColor);
				mcc->insert("gridColor", g_gridColor);
				mcc->insert("maskPath", g_maskPath.generic_u8string());
				root->insert("maskcolorcheckerproject", mcc);
				file << *root;
				file.close();
				g_projectPath = toSaveTo;
				g_recentProjects.push_back(toSaveTo);
			}
			g_unsaved = false;
			g_menuItemSave->deactivate();
		}
	}
	else if (e == CB_OPEN)
	{
		Fl_Native_File_Chooser openFileChooser;
		openFileChooser.title("Open project file");
		openFileChooser.filter("TOML file\t*.toml");
		openFileChooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
		if (openFileChooser.show() == 0)
		{
			g_faultyTiles.clear();
			std::filesystem::path toOpen = std::filesystem::u8path(openFileChooser.filename());
			toOpen.replace_extension(".toml");
			std::shared_ptr<cpptoml::table> root;
			try
			{
				root = cpptoml::parse_file(toOpen.u8string());
			}
			catch (const cpptoml::parse_exception& e)
			{
				fl_message_title("Error");
				fl_alert("Failed to load project file");
				return;
			}

			std::shared_ptr<cpptoml::table> mcc = root->get_table("maskcolorcheckerproject");
			if (mcc == nullptr)
			{
				fl_message_title("Error");
				fl_alert("Failed to load project file");
				return;
			}

			g_tileSize = mcc->get_as<unsigned int>("tileSize").value_or(256);
			g_overlap = mcc->get_as<unsigned int>("actualOverlap").value_or(0);
			g_tilesCount = mcc->get_as<unsigned int>("tilesInRow").value_or(0);
			g_colorsPerTile = mcc->get_as<unsigned int>("colorsPerTile").value_or(0);
			g_badTileColor = mcc->get_as<unsigned int>("badTileColor").value_or(16777216);
			g_gridColor = mcc->get_as<unsigned int>("gridColor").value_or(16777216);
			
			g_maskPath = std::filesystem::u8path(mcc->get_as<std::string>("maskPath").value_or(""));

			if (g_maskPath.empty() &&
				(g_tileSize != 128 && g_tileSize != 256 && g_tileSize != 512 && g_tileSize != 1024 && g_tileSize != 2048) &&
				(g_overlap == 0 || g_tilesCount == 0 || g_colorsPerTile == 0 || g_badTileColor == 16777216 || g_gridColor == 16777216))
			if (mcc == nullptr)
			{
				fl_message_title("Error");
				fl_alert("Failed to load project file");
				return;
			}

			switch (g_tileSize)
			{
			case 128:
				g_tileSizeChoice->value(0);
				break;
			case 256:
				g_tileSizeChoice->value(1);
				break;
			case 512:
				g_tileSizeChoice->value(2);
				break;
			case 1024:
				g_tileSizeChoice->value(3);
				break;
			case 2048:
				g_tileSizeChoice->value(4);
				break;
			}

			g_overlapSpinner->value(g_overlap);
			g_tilesInRowSpinner->value(g_tilesCount);
			g_colorsPerTileSpinner->value(g_colorsPerTile);
			g_badTileFillColorButton->color(g_badTileColor);
			g_gridColorButton->color(g_gridColor);

			int n;
			if (g_maskImg != nullptr)
				stbi_image_free(g_maskImg);
			g_maskImg = stbi_load(g_maskPath.u8string().c_str(), &g_maskImgX, &g_maskImgY, &n, 3);
			if (g_maskImg == nullptr)
			{
				fl_message_title("Error");
				fl_alert("Failed to load project file");
				return;
			}
			g_canvas->set(g_maskImg, g_maskImgX, g_maskImgY, &g_faultyTiles);
			g_maskPathOutput->value(g_maskPath.u8string().c_str());
			CheckParametersValid();
			g_projectPath = toOpen;
		}
	}
}

Fragment::Fragment()
{
	x = 0;
	y = 0;
	w = 0;
	h = 0;
}

Fragment::Fragment(unsigned int apx, unsigned int apy, unsigned int asx, unsigned int asy)
{
	x = apx;
	y = apy;
	w = asx;
	h = asy;
}

// Extend this fragment by the other fragment.
// This is used to prevent needlessly dividing
// corner or edge tiles
void Fragment::Extend(Fragment& other)
{
	if (other.x + other.w > x + w) w += other.w;
	if (other.y + other.h > y + h) h += other.h;
	unsigned int dx = x - other.x;
	unsigned int dy = y - other.y;
	if (other.x < x || other.y < y)
	{
		if (other.x < x)
		{
			w += dx;
			x = other.x;
		}
		if (other.y < y)
		{
			h += dy;
			y = other.y;
		}
	}
}

Tile::Tile(unsigned int ax, unsigned int ay, unsigned int apx, unsigned int apy, unsigned int asx, unsigned int asy)
{
	x = ax;
	y = ay;
	startX = apx;
	startY = apy;
	sizeX = asx;
	sizeY = asy;

	NW = Fragment(startX, startY, g_overlap, g_overlap);
	N =  Fragment(startX + g_overlap, startY, sizeX - (g_overlap << 1), g_overlap);
	NE = Fragment(startX + sizeX - g_overlap, startY, g_overlap, g_overlap);
	W =  Fragment(startX, startY + g_overlap, g_overlap, sizeY - (g_overlap << 1));
	M =  Fragment(startX + g_overlap, startY + g_overlap, sizeX - (g_overlap << 1), sizeY - (g_overlap << 1));
	E =  Fragment(startX + sizeX - g_overlap, startY + g_overlap, g_overlap, sizeY - (g_overlap << 1));
	SW = Fragment(startX, startY + sizeY - g_overlap, g_overlap, g_overlap);
	S =  Fragment(startX + g_overlap, startY + sizeY - g_overlap, sizeX - (g_overlap << 1), g_overlap);
	SE = Fragment(startX + sizeX - g_overlap, startY + sizeY - g_overlap, g_overlap, g_overlap);

	// For edges and corners, merge some fragments
	// Setting the width of the fragment to 0 disables it.
	if (y == 0) // Top row
	{
		if (x == 0) // Top left
		{
			M.Extend(NW);
			NW.w = 0;
			N.w = 0;
			W.w = 0;
			S.Extend(SW);
			SW.w = 0;
			E.Extend(NE);
			NE.w = 0;
		}
		else if (x == g_tilesCount - 1) // Top right
		{
			M.Extend(NE);
			NE.w = 0;
			N.w = 0;
			E.w = 0;
			S.Extend(SE);
			SE.w = 0;
			W.Extend(NW);
			NW.w = 0;
		}
		else // Top middle
		{
			W.Extend(NW);
			NW.w = 0;
			M.Extend(N);
			N.w = 0;
			E.Extend(NW);
			NW.w = 0;
		}
	}
	else if (y == g_tilesCount - 1) // Bottom row
	{
		if (x == 0) // Bottom left
		{
			M.Extend(SW);
			SW.w = 0;
			W.w = 0;
			S.w = 0;
			N.Extend(NW);
			NW.w = 0;
			E.Extend(SE);
			SE.w = 0;
		}
		else if (x == g_tilesCount - 1) // Bottom right
		{
			M.Extend(SE);
			SE.w = 0;
			E.w = 0;
			S.w = 0;
			N.Extend(NE);
			NE.w = 0;
			W.Extend(SW);
			SW.w = 0;
		}
		else // Bottom middle
		{
			W.Extend(SW);
			SW.w = 0;
			M.Extend(S);
			S.w = 0;
			E.Extend(SE);
			SE.w = 0;
		}
	}
	else // All the rows in the middle
	{
		if (x == 0) // Leftmost column sans top and bottom tile
		{
			N.Extend(NW);
			NW.w = 0;
			M.Extend(W);
			W.w = 0;
			S.Extend(SW);
			SW.w = 0;
		}
		else if (x == g_tilesCount - 1) // Rightmost column sans top and bottom tile
		{
			N.Extend(NE);
			NE.w = 0;
			M.Extend(E);
			E.w = 0;
			S.Extend(SE);
			SE.w = 0;
		}
	}
}

Tile::~Tile()
{

}