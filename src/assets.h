#ifndef ASSETS_H
#define ASSETS_H
#include "config.h"
#include "raylib.h"
typedef struct
{
	char filename[256];
	Music music;
} MusicFile;
typedef struct
{
	Texture2D tileset;
	int tileSize;
	Sound jumpSound;
	Sound levelCompleteSound;
	MusicFile musicFiles[MAX_MUSIC_FILES];
	int musicFileCount;
} Assets;
void Assets_Load(Assets *assets);
void Assets_Unload(Assets *assets);
Rectangle Assets_GetTileSource(int tileType);
void Assets_PlayMusic(Assets *assets, const char *filename);
void Assets_StopMusic(Assets *assets);
void Assets_UpdateMusic(Assets *assets);
void Assets_PlayJumpSound(Assets *assets);
void Assets_PlayLevelCompleteSound(Assets *assets);
int Assets_GetMusicCount(const Assets *assets);
const char *Assets_GetMusicFilename(const Assets *assets, int index);
#endif