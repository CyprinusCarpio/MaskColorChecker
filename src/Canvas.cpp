#include "Canvas.h"
#include "Program.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#include <string>
#include <iostream>


const float zoomLevels[] = {0.05f, 0.1f, 0.125f, 0.25f, 0.35f, 0.5f, 0.75f, 0.90f, 1.f, 1.25f, 1.5f, 1.75f, 2.f, 3.f};


Canvas::Canvas(int x, int y, int w, int h) : Fl_Gl_Window(x, y, w, h)
{
	
}

Canvas::~Canvas()
{
	if (maskImage != nullptr)
	{
		glDeleteTextures(1, &textureID);
		if (maskImageScale != 1.f)
		{
			delete[] maskImage;
		}
	}
}

void Canvas::draw()
{
	if (!valid())
	{
		glMatrixMode(GL_PROJECTION);
		glViewport(0.f, 0.f, pixel_w(), pixel_h());
		glLoadIdentity();
		glOrtho(0.0, pixel_w(), pixel_h(), 0.0, 1.0, -1.0);
		glClearColor(0.f, 0.f, 0.f, 1.f);
	}

	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT);

	if (maskImage == nullptr)
		return;

	if (textureID == 0)
	{
		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, maskImgX, maskImgY, 0, GL_RGB, GL_UNSIGNED_BYTE, maskImage);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	glPushMatrix();
	glTranslatef(pixel_w() / 2.0f, pixel_h() / 2.0f, 0.0f);
	glScalef(zoomLevels[zoom], zoomLevels[zoom], 0);
	glTranslatef(panX, panY, 0);

	glBindTexture(GL_TEXTURE_2D, textureID);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(maskImgX * maskImageScale, 0.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(maskImgX * maskImageScale, maskImgY * maskImageScale);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(0.0f, maskImgY * maskImageScale);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	
	if (g_drawGrid)
	{
		drawGrid();
	}

	gl_color(g_badTileColor);
	gl_font(FL_HELVETICA_BOLD, 14);

	drawFaultyTiles();

	glPopMatrix();

	std::string zoomText = "zoom = " + std::to_string(zoomLevels[zoom]) + " panX = " + std::to_string(panX) + " panY = " + std::to_string(panY);
	gl_draw(zoomText.c_str(), 10.f, 15.f);

	std::string imgText = "imgPosX = " + std::to_string(imgMousePosX) + " imgPosY = " + std::to_string(imgMousePosY);
	gl_draw(imgText.c_str(), 10.f, 35.f);

	std::string screenText = "screenPosX = " + std::to_string(screenMousePosX) + " screenPosY = " + std::to_string(screenMousePosY);
	gl_draw(screenText.c_str(), 10.f, 55.f);

	if(imgMousePosY >= 0 && imgMousePosY < maskImgY && imgMousePosX > 0 && imgMousePosX < maskImgX)
	{
		std::string rgbText = "R = " + std::to_string((int)maskImage[imgMousePosY * maskImgY * 3 + imgMousePosX * 3]) +
			" G = " + std::to_string((int)maskImage[imgMousePosY * maskImgY * 3 + imgMousePosX * 3 + 1]) +
			" B = " + std::to_string((int)maskImage[imgMousePosY * maskImgY * 3 + imgMousePosX * 3 + 2]);

		gl_draw(rgbText.c_str(), 10.f, 75.f);
	}
	
	glBegin(GL_LINES);
		glVertex2f(pixel_w() / 2 - 6, pixel_h() / 2);
		glVertex2f(pixel_w() / 2 + 5, pixel_h() / 2);
		glVertex2f(pixel_w() / 2, pixel_h() / 2 - 5);
		glVertex2f(pixel_w() / 2, pixel_h() / 2 + 6);
	glEnd();

	gl_color(FL_WHITE);
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

		redraw();
		return 1;
	}
	
	else if (e == FL_ENTER)
	{
		//cursor(FL_CURSOR_CROSS);
		return 1;
	}

	else if (e == FL_MOVE)
	{
		imgMousePosX = Fl::event_x() - (pixel_w() / 2.f);
		imgMousePosY = Fl::event_y() - (pixel_h() / 2.f);
		imgMousePosX /= zoomLevels[zoom];
		imgMousePosY /= zoomLevels[zoom];
		imgMousePosX -= panX;
		imgMousePosY -= panY;
		screenMousePosX = Fl::event_x();
		screenMousePosY = Fl::event_y();
		redraw();
	}

	else if (e == FL_PUSH)
	{
		if (Fl::event_button() == FL_LEFT_MOUSE)
		{
			imgMousePosX = Fl::event_x() - (pixel_w() / 2.f);
			imgMousePosY = Fl::event_y() - (pixel_h() / 2.f);
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
			prevPanX = Fl::event_x();
			prevPanY = Fl::event_y();
			isPanning = true;
			cursor(FL_CURSOR_MOVE);
			return 1;
		}
	}

	else if (e == FL_DRAG)
	{
		if(isPanning)
		{
			panX += (Fl::event_x() - prevPanX) / zoomLevels[zoom];
			panY += (Fl::event_y() - prevPanY) / zoomLevels[zoom];
			prevPanX = Fl::event_x();
			prevPanY = Fl::event_y();
			redraw();
		}
		return 1;
	}

	else if (e == FL_RELEASE)
	{
		if (Fl::event_button() == FL_RIGHT_MOUSE)
		{
			isPanning = false;
			//cursor(FL_CURSOR_CROSS);
			cursor(FL_CURSOR_DEFAULT);
			return 1;
		}
	}

	return Fl_Gl_Window::handle(e);
}

void Canvas::drawGrid()
{
	gl_color(g_gridColor);

	int spacing = g_tileSize - g_overlap;
	for (int x = 0; x < g_tilesCount; x++)
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
			tileWidth = (maskImgX * maskImageScale) - xPos + 1;
		}

		for (int y = 0; y < g_tilesCount; y++)
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
				tileHeight = (maskImgY * maskImageScale) - yPos + 1;
			}

			gl_rect(xPos, yPos, tileWidth, tileHeight);
		}
	}
}

void Canvas::drawFaultyTiles()
{
	int spacing = g_tileSize - g_overlap;

	for (Tile& tile : *faultyTiles)
	{
		int x = tile.x;
		int y = tile.y;
		int tileWidth = g_tileSize;
		int xPos = (spacing * x) - (g_overlap / 2.f);
		if (x == 0)
		{
			tileWidth = g_tileSize - (g_overlap / 2.f);
			xPos = 0;
		}
		else if (x == g_tilesCount - 1)
		{
			tileWidth = (maskImgX * maskImageScale) - xPos + 1;
		}
		int tileHeight = g_tileSize;

		int yPos = (spacing * y) - (g_overlap / 2.f);
		if (y == 0)
		{
			tileHeight = g_tileSize - (g_overlap / 2.f);
			yPos = 0;
		}
		else if (y == g_tilesCount - 1)
		{
			tileHeight = (maskImgY * maskImageScale) - yPos + 1;
		}

		if (g_solidFill)
		{
			gl_rectf(xPos, yPos, tileWidth, tileHeight);
			if (zoom >= 4)
			{
				gl_color(fl_contrast(0, g_badTileColor));
				std::string toDraw = std::to_string(x) + " / " + std::to_string(y);
				gl_draw(toDraw.c_str(), xPos + 20.f, yPos + 40.f);
				gl_color(g_badTileColor);
			}
		}
		else
		{
			gl_rect(xPos, yPos, tileWidth, tileHeight);
			if (zoom >= 4)
			{
				std::string toDraw = std::to_string(x) + " / " + std::to_string(y);
				gl_draw(toDraw.c_str(), xPos + 20.f, yPos + 40.f);
			}
		}
	}

}

void Canvas::set(unsigned char* img, int mX, int mY, std::deque<Tile>* fT)
{
	if (maskImage != nullptr)
	{
		glDeleteTextures(1, &textureID);
		textureID = 0;
		// If maskImageScale isn't 1.0 then we know we have a rescaled copy
		// Needs to be deleted to avoid a memory leak.
		if (maskImageScale != 1.f)
		{
			delete[] maskImage;
		}
	}
	faultyTiles = fT;
	maskImgX = mX;
	maskImgY = mY;
	maskImage = nullptr;
	// If the mask size exceeds the max GL texture size, it would be displayed as
	// a solid white rectangle. To fix this we draw a copy rescaled to 4096x4096 instead
	GLint maxTextureSize = 0;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
	if (maskImgX > maxTextureSize)
	{
		maskImageScale = (float)maskImgX / 4096;
		maskImage = stbir_resize_uint8_srgb(img, maskImgX, maskImgY, 0, nullptr, 4096, 4096, 0, STBIR_RGB);
		maskImgX = 4096;
		maskImgY = 4096;
	}
	else
	{
		maskImage = img;
		maskImageScale = 1.f;
	}
}
