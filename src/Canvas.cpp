#include "Canvas.h"
#include "Program.h"

#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include <string>

const float zoomLevels[] = {0.05f, 0.1f, 0.125f, 0.25f, 0.35f, 0.5f, 0.75f, 0.90f, 1.f, 1.25f, 1.5f, 1.75f, 2.f, 3.f};

Canvas::Canvas(int X, int Y, int W, int H) : Fl_Box(X, Y, W, H)
{
	Fl_Image::scaling_algorithm(FL_RGB_SCALING_NEAREST);
}

Canvas::~Canvas()
{
	if (rgb != nullptr) delete rgb;
}

void Canvas::draw()
{
	// Clear the canvas
	Fl_Box::draw();

	if (rgb == nullptr)
		return;
	
	// stop drawing outside of the widget
	fl_push_clip(x(), y(), w(), h());

	rgb->draw(x() + (panX * zoomLevels[zoom]) + (w() / 2.f), y() + (panY * zoomLevels[zoom]) + (h() / 2.f));
	
	if (g_drawGrid)
	{
		drawGrid();
	}

	fl_color(g_badTileColor);
	fl_font(FL_HELVETICA_BOLD, 14);

	drawFaultyTiles();

	std::string zoomText = "zoom = " + std::to_string(zoomLevels[zoom]) + " panX = " + std::to_string(panX) + " panY = " + std::to_string(panY);
	fl_draw(zoomText.c_str(), x() + 10.f, y() + 15.f);

	std::string imgText = "imgPosX = " + std::to_string(imgMousePosX) + " imgPosY = " + std::to_string(imgMousePosY);
	fl_draw(imgText.c_str(), x() + 10.f, y() + 35.f);

	std::string screenText = "screenPosX = " + std::to_string(screenMousePosX) + " screenPosY = " + std::to_string(screenMousePosY);
	fl_draw(screenText.c_str(), x() + 10.f, y() + 55.f);

	if(imgMousePosY >= 0 && imgMousePosY < maskImgY && imgMousePosX > 0 && imgMousePosX < maskImgX)
	{
		const char* const data = *rgb->data();
		std::string rgbText = "R = " + std::to_string((int)data[imgMousePosY * maskImgY * 3 + imgMousePosX * 3]) +
			" G = " + std::to_string((int)data[imgMousePosY * maskImgY * 3 + imgMousePosX * 3 + 1]) +
			" B = " + std::to_string((int)data[imgMousePosY * maskImgY * 3 + imgMousePosX * 3 + 2]);

		fl_draw(rgbText.c_str(), x() + 10.f, y() + 75.f);
	}

	// Crosshair
	fl_line(w() / 2 - 6 + x(), h() / 2 + y(), w() / 2 + 5 + x(), h() / 2 + y());
	fl_line(w() / 2 + x(), h() / 2 - 5 + y(), w() / 2 + x(), h() / 2 + 6 + y());

	fl_color(FL_WHITE);
	fl_pop_clip();
}

int Canvas::handle(int e)
{
	if (e == FL_MOUSEWHEEL)
	{
		if (Fl::event_dy() < 0)
		{
			zoom++;
			if (zoom > 13) zoom = 13;
		}
		else
		{
			zoom--;
			if (zoom < 0) zoom = 0;
		}
		rgb->scale(rgb->data_w() * zoomLevels[zoom], rgb->data_h() * zoomLevels[zoom], true, true);
		redraw();
		return 1;
	}
	
	else if (e == FL_ENTER)
	{
		return 1;
	}

	else if (e == FL_MOVE)
	{
		imgMousePosX = (Fl::event_x() - x()) - (w() / 2.f);
		imgMousePosY = (Fl::event_y() - y()) - (h() / 2.f);
		imgMousePosX /= zoomLevels[zoom];
		imgMousePosY /= zoomLevels[zoom];
		imgMousePosX -= panX;
		imgMousePosY -= panY;
		screenMousePosX = (Fl::event_x() - x());
		screenMousePosY = (Fl::event_y() - y());
		redraw();
	}

	else if (e == FL_PUSH)
	{
		if (Fl::event_button() == FL_LEFT_MOUSE)
		{
			imgMousePosX = (Fl::event_x() - x()) - (w() / 2.f);
			imgMousePosY = (Fl::event_y() - y()) - (h() / 2.f);
			imgMousePosX /= zoomLevels[zoom];
			imgMousePosY /= zoomLevels[zoom];
			imgMousePosX -= panX;
			imgMousePosY -= panY;

			panX = -imgMousePosX;
			panY = -imgMousePosY;
			redraw();
		}
		if (Fl::event_button() == FL_RIGHT_MOUSE && !isPanning)
		{
			prevPanX = Fl::event_x() - x();
			prevPanY = Fl::event_y() - y();
			isPanning = true;
			window()->cursor(FL_CURSOR_MOVE);
			return 1;
		}
	}

	else if (e == FL_DRAG)
	{
		if(isPanning)
		{
			panX += ((Fl::event_x() - x()) - prevPanX) / zoomLevels[zoom];
			panY += ((Fl::event_y() - y()) - prevPanY) / zoomLevels[zoom];
			prevPanX = Fl::event_x() - x();
			prevPanY = Fl::event_y() - y();
			redraw();
		}
		return 1;
	}

	else if (e == FL_RELEASE)
	{
		if (Fl::event_button() == FL_RIGHT_MOUSE)
		{
			isPanning = false;
			window()->cursor(FL_CURSOR_DEFAULT);
			return 1;
		}
	}

	return Fl_Widget::handle(e);
}

void Canvas::drawGrid()
{
	fl_color(g_gridColor);

	int spacing = g_tileSize - g_overlap;
	for (int X = 0; X < g_tilesCount; X++)
	{
		int tileWidth = g_tileSize;
		int xPos = (spacing * X) - (g_overlap / 2.f);
		if (X == 0)
		{
			tileWidth = g_tileSize - (g_overlap / 2.f);
			xPos = 0;
		}
		else if (X == g_tilesCount - 1)
		{
			tileWidth = maskImgX - xPos + 1;
		}

		for (int Y = 0; Y < g_tilesCount; Y++)
		{
			int tileHeight = g_tileSize;

			int yPos = (spacing * Y) - (g_overlap / 2.f);
			if (Y == 0)
			{
				tileHeight = g_tileSize - (g_overlap / 2.f);
				yPos = 0;
			}
			else if (Y == g_tilesCount - 1)
			{
				tileHeight = maskImgY - yPos + 1;
			}

			fl_rect((xPos * zoomLevels[zoom]) + x() + (panX * zoomLevels[zoom]) + (w() / 2.f),
				    (yPos * zoomLevels[zoom]) + y() + (panY * zoomLevels[zoom]) + (h() / 2.f),
				    tileWidth * zoomLevels[zoom], tileHeight * zoomLevels[zoom]);
		}
	}
}

void Canvas::drawFaultyTiles()
{
	int spacing = g_tileSize - g_overlap;

	for (Tile& tile : *faultyTiles)
	{
		int X = tile.x;
		int Y = tile.y;
		int tileWidth = g_tileSize;
		int xPos = (spacing * X) - (g_overlap / 2.f);
		if (X == 0)
		{
			tileWidth = g_tileSize - (g_overlap / 2.f);
			xPos = 0;
		}
		else if (X == g_tilesCount - 1)
		{
			tileWidth = maskImgX - xPos + 1;
		}
		int tileHeight = g_tileSize;

		int yPos = (spacing * Y) - (g_overlap / 2.f);
		if (Y == 0)
		{
			tileHeight = g_tileSize - (g_overlap / 2.f);
			yPos = 0;
		}
		else if (Y == g_tilesCount - 1)
		{
			tileHeight = maskImgY - yPos + 1;
		}

		float textOffX = 20.f * zoomLevels[zoom];
		float textOffY = 40.f * zoomLevels[zoom];

		if (g_solidFill)
		{
			fl_rectf((xPos * zoomLevels[zoom]) + x() + (panX * zoomLevels[zoom]) + (w() / 2.f),
				    (yPos * zoomLevels[zoom]) + y() + (panY * zoomLevels[zoom]) + (h() / 2.f),
				    tileWidth * zoomLevels[zoom], tileHeight * zoomLevels[zoom]);
			if (zoom >= 4)
			{
				fl_color(fl_contrast(0, g_badTileColor));
				std::string toDraw = std::to_string(X) + " / " + std::to_string(Y);
				fl_draw(toDraw.c_str(), (xPos * zoomLevels[zoom]) + x() + (panX * zoomLevels[zoom]) + (w() / 2.f) + textOffX,
					   (yPos * zoomLevels[zoom]) + y() + (panY * zoomLevels[zoom]) + (h() / 2.f) + textOffY);
				fl_color(g_badTileColor);
			}
		}
		else
		{
			fl_rect((xPos * zoomLevels[zoom]) + x() + (panX * zoomLevels[zoom]) + (w() / 2.f),
				   (yPos * zoomLevels[zoom]) + y() + (panY * zoomLevels[zoom]) + (h() / 2.f),
				   tileWidth * zoomLevels[zoom], tileHeight * zoomLevels[zoom]);
			if (zoom >= 4)
			{
				std::string toDraw = std::to_string(X) + " / " + std::to_string(Y);
				fl_draw(toDraw.c_str(), (xPos * zoomLevels[zoom]) + x() + (panX * zoomLevels[zoom]) + (w() / 2.f) + textOffX,
					yPos * zoomLevels[zoom] + y() + (panY * zoomLevels[zoom]) + (h() / 2.f) + textOffY);
			}
		}
	}

}

void Canvas::set(unsigned char* img, int mX, int mY, std::deque<Tile>* fT)
{
	if (rgb != nullptr)
	{
		delete rgb;
	}
	faultyTiles = fT;
	maskImgX = mX;
	maskImgY = mY;
	rgb = new Fl_RGB_Image(img, maskImgX, maskImgY);
}
