#ifndef LEVEL_H
#define LEVEL_H
#include "assets.h"
#include "config.h"
#include "raylib.h"
typedef struct
{
	char characterName[32];
	char text[256];
	char characterSprite[64];
	int bgColor;
} VNDialogue;
typedef struct
{
	int width, height;
	int **tiles;
	int **backgroundTiles;
	Vector2 playerSpawn;
	Vector2 goalPos;
	bool hasGoal;
	char musicFile[256];
	bool hasVisualNovel;
	VNDialogue dialogues[20];
	int dialogueCount;
} Level;
void Level_Load(Level *lvl, int index);
void Level_LoadFromFile(Level *lvl, const char *filepath);
void Level_SaveToFile(const Level *lvl, const char *filepath);
void Level_Create(Level *lvl, int width, int height);
void Level_Unload(Level *lvl);
void Level_Draw(const Level *lvl, const Assets *assets, Camera2D camera);
bool Level_IsSolid(const Level *lvl, int tx, int ty);
int Level_GetTile(const Level *lvl, int tx, int ty);
void Level_SetTile(Level *lvl, int tx, int ty, int tileType);
Rectangle Level_GetTileBounds(int tx, int ty);
int Level_CountFiles(void);
TileEffect Level_GetTileEffect(int tileType);
#endif
