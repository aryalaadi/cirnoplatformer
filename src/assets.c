#include "assets.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
void Assets_Load(Assets *assets) {
  assets->tileSize = 50;
  assets->musicFileCount = 0;
  if (FileExists("assets/tiles/tileset.png")) {
    assets->tileset = LoadTexture("assets/tiles/tileset.png");
  } else {
    Image img = GenImageColor(250, 50, BLANK);
    ImageDrawRectangle(&img, 0, 0, 50, 50, GREEN);
    ImageDrawRectangle(&img, 0, 40, 50, 10, DARKGREEN);
    ImageDrawRectangle(&img, 50, 0, 50, 50, (Color){139, 69, 19, 255});
    ImageDrawRectangle(&img, 100, 0, 50, 50, GRAY);
    ImageDrawRectangle(&img, 105, 5, 15, 15, DARKGRAY);
    ImageDrawRectangle(&img, 130, 25, 12, 12, DARKGRAY);
    ImageDrawRectangle(&img, 150, 0, 50, 50, GOLD);
    ImageDrawRectangle(&img, 160, 10, 30, 30, YELLOW);
    assets->tileset = LoadTextureFromImage(img);
    UnloadImage(img);
  }
  if (FileExists("assets/sounds/jump.wav")) {
    assets->jumpSound = LoadSound("assets/sounds/jump.wav");
  } else if (FileExists("assets/sounds/jump.ogg")) {
    assets->jumpSound = LoadSound("assets/sounds/jump.ogg");
  } else {
    Wave wave = {0};
    wave.frameCount = 4000;
    wave.sampleRate = 44100;
    wave.sampleSize = 16;
    wave.channels = 1;
    assets->jumpSound = LoadSoundFromWave(wave);
  }
  if (FileExists("assets/sounds/complete.wav")) {
    assets->levelCompleteSound = LoadSound("assets/sounds/complete.wav");
  } else if (FileExists("assets/sounds/complete.ogg")) {
    assets->levelCompleteSound = LoadSound("assets/sounds/complete.ogg");
  } else {
    Wave wave = {0};
    wave.frameCount = 8000;
    wave.sampleRate = 44100;
    wave.sampleSize = 16;
    wave.channels = 1;
    assets->levelCompleteSound = LoadSoundFromWave(wave);
  }
  FilePathList musicFiles = LoadDirectoryFiles("assets/music");
  for (unsigned int i = 0;
       i < musicFiles.count && assets->musicFileCount < MAX_MUSIC_FILES; i++) {
    const char *ext = GetFileExtension(musicFiles.paths[i]);
    if (TextIsEqual(ext, ".ogg") || TextIsEqual(ext, ".mp3") ||
        TextIsEqual(ext, ".wav")) {
      strncpy(assets->musicFiles[assets->musicFileCount].filename,
              GetFileName(musicFiles.paths[i]), 255);
      assets->musicFiles[assets->musicFileCount].music =
          LoadMusicStream(musicFiles.paths[i]);
      assets->musicFileCount++;
    }
  }
  UnloadDirectoryFiles(musicFiles);
}
void Assets_Unload(Assets *assets) {
  UnloadTexture(assets->tileset);
  UnloadSound(assets->jumpSound);
  UnloadSound(assets->levelCompleteSound);
  for (int i = 0; i < assets->musicFileCount; i++) {
    UnloadMusicStream(assets->musicFiles[i].music);
  }
  assets->musicFileCount = 0;
}
Rectangle Assets_GetTileSource(int tileType) {
  if (tileType < 0)
    return (Rectangle){0, 0, 0, 0};
  int tilesPerRow = 20;
  int col = tileType % tilesPerRow;
  int row = tileType / tilesPerRow;
  return (Rectangle){col * 50.0f, row * 50.0f, 50, 50};
}
void Assets_PlayMusic(Assets *assets, const char *filename) {
  Assets_StopMusic(assets);
  for (int i = 0; i < assets->musicFileCount; i++) {
    if (strcmp(assets->musicFiles[i].filename, filename) == 0) {
      PlayMusicStream(assets->musicFiles[i].music);
      return;
    }
  }
}
void Assets_StopMusic(Assets *assets) {
  for (int i = 0; i < assets->musicFileCount; i++) {
    StopMusicStream(assets->musicFiles[i].music);
  }
}
void Assets_UpdateMusic(Assets *assets) {
  for (int i = 0; i < assets->musicFileCount; i++) {
    UpdateMusicStream(assets->musicFiles[i].music);
  }
}
void Assets_PlayJumpSound(Assets *assets) { PlaySound(assets->jumpSound); }
void Assets_PlayLevelCompleteSound(Assets *assets) {
  PlaySound(assets->levelCompleteSound);
}
int Assets_GetMusicCount(const Assets *assets) {
  return assets->musicFileCount;
}
const char *Assets_GetMusicFilename(const Assets *assets, int index) {
  if (index >= 0 && index < assets->musicFileCount) {
    return assets->musicFiles[index].filename;
  }
  return "";
}