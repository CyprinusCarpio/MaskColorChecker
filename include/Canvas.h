#ifndef CANVAS_H
#define CANVAS_H

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>

#include <deque>

struct Tile;

class Canvas : public Fl_Gl_Window
{
	int zoom = 8;
	//float zoom = 1.f;
	float panX = 0.f;
	float panY = 0.f;
	int prevPanX = 0;
	int prevPanY = 0;
	bool isPanning = false;
	int maskImgX = 0;
	int maskImgY = 0;

	int imgMousePosX = 0;
	int imgMousePosY = 0;
	int screenMousePosX = 0;
	int screenMousePosY = 0;

	GLuint textureID = 0;
	unsigned char* maskImage = nullptr;
	float maskImageScale = 1.f;

	std::deque<Tile>* faultyTiles = nullptr;
public:
	Canvas(int x, int y, int w, int h);
	~Canvas();

	void draw() override;
	int handle(int e) override;

	void drawFaultyTiles();
	void drawGrid();
	void set(unsigned char* img, int mX, int mY, std::deque<Tile> *fT);
};

#endif
