#ifndef PROGRAM_H
#define PROGRAM_H

#include <string>
#include <filesystem>

#define CB_OPEN          100
#define CB_SAVE          101
#define CB_SAVEAS        102
#define CB_QUIT          103
#define CB_HELP          120
#define CB_ABOUT         121
#define CB_TILESIZE      200
#define CB_OVERLAP       201
#define CB_TILECOUNT     202
#define CB_COLORSPERTILE 203
#define CB_BADTILECOLOR  204
#define CB_SOLIDFILL     205
#define CB_LOADMASK      206
#define CB_CHECK         207
#define CB_SAVEOUTPUT    208
#define CB_GRIDCOLOR     209
#define CB_DRAWGRID      210
#define CB_FIX           211
#define CB_SAVEGRID      212

struct Fragment
{
	//Fragments are considered disabled if w == 0
	unsigned int x, y, w, h;

	Fragment();
	Fragment(unsigned int apx, unsigned int apy, unsigned int asx, unsigned int asy);

	void Extend(Fragment& other);
};

struct Tile
{
	unsigned int x, y, startX, startY, sizeX, sizeY;

	Fragment NW;
	Fragment N;
	Fragment NE;
	Fragment W;
	Fragment M;
	Fragment E;
	Fragment SW;
	Fragment S;
	Fragment SE;

	Tile(unsigned int ax, unsigned int ay, unsigned int apx, unsigned int apy, unsigned int asx, unsigned int asy);
	~Tile();
};

extern unsigned int g_tileSize;
extern unsigned int g_desiredOverlap;
extern unsigned int g_overlap;
extern unsigned int g_tilesCount;
extern unsigned int g_colorsPerTile;
extern unsigned int g_badTileColor;
extern unsigned int g_gridColor;
extern bool         g_solidFill;
extern bool         g_drawGrid;

extern std::filesystem::path  g_maskPath;

void InitializeProgram();
void ShutdownProgram();
void OnGui(int e);

#endif