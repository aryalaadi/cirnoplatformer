/*
 * Cirno's Hardest Platformer 2026 - A challenging 2D platformer game.
 * Copyright (C) 2026 Aaditya Aryal
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "level.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void Level_Create(Level *lvl, int width, int height) {
  if (width < MIN_WORLD_WIDTH)
    width = MIN_WORLD_WIDTH;
  if (height < MIN_WORLD_HEIGHT)
    height = MIN_WORLD_HEIGHT;
  if (width > MAX_WORLD_WIDTH)
    width = MAX_WORLD_WIDTH;
  if (height > MAX_WORLD_HEIGHT)
    height = MAX_WORLD_HEIGHT;
  lvl->width = width;
  lvl->height = height;
  lvl->playerSpawn = (Vector2){100, 100};
  lvl->hasGoal = false;
  lvl->musicFile[0] = '\0';
  lvl->hasVisualNovel = false;
  lvl->dialogueCount = 0;
  lvl->tiles = (int **)malloc(lvl->height * sizeof(int *));
  lvl->backgroundTiles = (int **)malloc(lvl->height * sizeof(int *));
  for (int y = 0; y < lvl->height; y++) {
    lvl->tiles[y] = (int *)calloc(lvl->width, sizeof(int));
    lvl->backgroundTiles[y] = (int *)calloc(lvl->width, sizeof(int));
  }
}
int Level_CountFiles(void) {
  FilePathList files = LoadDirectoryFiles("assets/levels");
  int count = 0;
  for (unsigned int i = 0; i < files.count; i++) {
    const char *ext = GetFileExtension(files.paths[i]);
    if (TextIsEqual(ext, ".lvl")) {
      count++;
    }
  }
  UnloadDirectoryFiles(files);
  return count;
}
void Level_LoadFromFile(Level *lvl, const char *filepath) {
  FILE *f = fopen(filepath, "rb");
  if (!f) {
    Level_Create(lvl, 30, 20);
    return;
  }
  fread(&lvl->width, sizeof(int), 1, f);
  fread(&lvl->height, sizeof(int), 1, f);
  fread(&lvl->playerSpawn, sizeof(Vector2), 1, f);
  fread(&lvl->goalPos, sizeof(Vector2), 1, f);
  fread(&lvl->hasGoal, sizeof(bool), 1, f);
  fread(lvl->musicFile, sizeof(char), 256, f);
  fread(&lvl->hasVisualNovel, sizeof(bool), 1, f);
  fread(&lvl->dialogueCount, sizeof(int), 1, f);
  if (lvl->hasVisualNovel && lvl->dialogueCount > 0) {
    fread(lvl->dialogues, sizeof(VNDialogue), lvl->dialogueCount, f);
  }
  lvl->tiles = (int **)malloc(lvl->height * sizeof(int *));
  lvl->backgroundTiles = (int **)malloc(lvl->height * sizeof(int *));
  for (int y = 0; y < lvl->height; y++) {
    lvl->tiles[y] = (int *)malloc(lvl->width * sizeof(int));
    lvl->backgroundTiles[y] = (int *)malloc(lvl->width * sizeof(int));
    fread(lvl->tiles[y], sizeof(int), lvl->width, f);
    fread(lvl->backgroundTiles[y], sizeof(int), lvl->width, f);
  }
  fclose(f);
}
void Level_SaveToFile(const Level *lvl, const char *filepath) {
  FILE *f = fopen(filepath, "wb");
  if (!f)
    return;
  fwrite(&lvl->width, sizeof(int), 1, f);
  fwrite(&lvl->height, sizeof(int), 1, f);
  fwrite(&lvl->playerSpawn, sizeof(Vector2), 1, f);
  fwrite(&lvl->goalPos, sizeof(Vector2), 1, f);
  fwrite(&lvl->hasGoal, sizeof(bool), 1, f);
  fwrite(lvl->musicFile, sizeof(char), 256, f);
  fwrite(&lvl->hasVisualNovel, sizeof(bool), 1, f);
  fwrite(&lvl->dialogueCount, sizeof(int), 1, f);
  if (lvl->hasVisualNovel && lvl->dialogueCount > 0) {
    fwrite(lvl->dialogues, sizeof(VNDialogue), lvl->dialogueCount, f);
  }
  for (int y = 0; y < lvl->height; y++) {
    fwrite(lvl->tiles[y], sizeof(int), lvl->width, f);
    fwrite(lvl->backgroundTiles[y], sizeof(int), lvl->width, f);
  }
  fclose(f);
}
static const char *level_templates[5] = {"000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000001110000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000011100"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000004"
                                         "000000111100000000000001111111"
                                         "000000000000000000000011111111"
                                         "000000000000000111111111111111"
                                         "011111111111111111111111111111"
                                         "111111111111111111111111111111",
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000400"
                                         "000000000000000111000000011100"
                                         "000000000000000000000000000000"
                                         "000000000111000000000000000000"
                                         "000000000000000000000011100000"
                                         "000000000000000000000000000000"
                                         "000000000000000111000000000000"
                                         "000000000000000000000000000000"
                                         "000011100000000000000000000000"
                                         "000000000000000000001110000000"
                                         "011100000000000000000000000000"
                                         "111111111111111111111111111111",
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000400"
                                         "000000000000000000000000001110"
                                         "000000000000000000000000000000"
                                         "000000000000000000111000000000"
                                         "000000000000000000000000000000"
                                         "000000000111100000000000000000"
                                         "000000000000000000000000000000"
                                         "000111100000000000000000000000"
                                         "000000000000000000000000000000"
                                         "011000000000000000000001100000"
                                         "111111111000000000001111111111",
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000400"
                                         "000000000000000000000000001110"
                                         "000000000000000000000000000000"
                                         "000000000000000000000111100000"
                                         "000000000000000000000000000000"
                                         "000000000000000011110000000000"
                                         "000000000000000000000000000000"
                                         "000000000001111000000000000000"
                                         "000000000000000000000000000000"
                                         "000000111100000000000000000000"
                                         "000000000000000000000000000000"
                                         "000111000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "011000000000000000000000000000"
                                         "110000000000000000000000000000"
                                         "100000000000000000000000000000"
                                         "111111111111111111111111111111",
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000000"
                                         "000000000000000000000000000400"
                                         "000000000000000000000000001110"
                                         "000000000000000000000000000000"
                                         "000000000000000000001100000000"
                                         "000000000000000000000000000000"
                                         "000000000000001110000000000000"
                                         "000000000000000000000000000000"
                                         "000000001100000000000000000000"
                                         "000000000000000000000000000000"
                                         "000011100000000000001100000000"
                                         "000000000000000000000000000000"
                                         "001100000000001100000000000000"
                                         "000000000000000000000000000000"
                                         "011000000110000000000000000000"
                                         "111111111111111111111111111111"};
void Level_Load(Level *lvl, int index) {
  FilePathList files = LoadDirectoryFiles("assets/levels");
  int levelCount = 0;
  char levelFiles[MAX_LEVELS][256];
  for (unsigned int i = 0; i < files.count && levelCount < MAX_LEVELS; i++) {
    const char *ext = GetFileExtension(files.paths[i]);
    if (TextIsEqual(ext, ".lvl")) {
      strncpy(levelFiles[levelCount], files.paths[i], 255);
      levelCount++;
    }
  }
  UnloadDirectoryFiles(files);
  if (index >= 0 && index < levelCount) {
    Level_LoadFromFile(lvl, levelFiles[index]);
    return;
  }
  if (index >= 0 && index < 5) {
    lvl->width = 30;
    lvl->height = 20;
    lvl->playerSpawn = (Vector2){100, 100};
    lvl->hasGoal = false;
    lvl->musicFile[0] = '\0';
    lvl->hasVisualNovel = false;
    lvl->dialogueCount = 0;
    lvl->tiles = (int **)malloc(lvl->height * sizeof(int *));
    lvl->backgroundTiles = (int **)malloc(lvl->height * sizeof(int *));
    for (int y = 0; y < lvl->height; y++) {
      lvl->tiles[y] = (int *)calloc(lvl->width, sizeof(int));
      lvl->backgroundTiles[y] = (int *)calloc(lvl->width, sizeof(int));
    }
    const char *template = level_templates[index];
    for (int y = 0; y < lvl->height; y++) {
      for (int x = 0; x < lvl->width; x++) {
        int idx = y * lvl->width + x;
        int tile = template[idx] - '0';
        lvl->tiles[y][x] = tile;
        if (tile == TILE_GOAL) {
          lvl->goalPos = (Vector2){x * TILE_SIZE, y * TILE_SIZE};
          lvl->hasGoal = true;
        }
      }
    }
  } else {
    Level_Create(lvl, 30, 20);
  }
}
void Level_Unload(Level *lvl) {
  if (lvl->tiles) {
    for (int y = 0; y < lvl->height; y++) {
      free(lvl->tiles[y]);
    }
    free(lvl->tiles);
    lvl->tiles = NULL;
  }
  if (lvl->backgroundTiles) {
    for (int y = 0; y < lvl->height; y++) {
      free(lvl->backgroundTiles[y]);
    }
    free(lvl->backgroundTiles);
    lvl->backgroundTiles = NULL;
  }
}
void Level_Draw(const Level *lvl, const Assets *assets, Camera2D camera) {
  int startX = (int)(camera.target.x - SCREEN_WIDTH / 2) / TILE_SIZE - 1;
  int endX = (int)(camera.target.x + SCREEN_WIDTH / 2) / TILE_SIZE + 2;
  int startY = (int)(camera.target.y - SCREEN_HEIGHT / 2) / TILE_SIZE - 1;
  int endY = (int)(camera.target.y + SCREEN_HEIGHT / 2) / TILE_SIZE + 2;
  if (startX < 0)
    startX = 0;
  if (endX > lvl->width)
    endX = lvl->width;
  if (startY < 0)
    startY = 0;
  if (endY > lvl->height)
    endY = lvl->height;
  for (int y = startY; y < endY; y++) {
    for (int x = startX; x < endX; x++) {
      int bgTile = lvl->backgroundTiles[y][x];
      if (bgTile > 0) {
        Rectangle dst = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        DrawRectangleRec(dst, (Color){100, 100, 150, 100});
        DrawText(TextFormat("%d", bgTile), x * TILE_SIZE + 5, y * TILE_SIZE + 5,
                 12, (Color){255, 255, 255, 150});
      }
    }
  }
  for (int y = startY; y < endY; y++) {
    for (int x = startX; x < endX; x++) {
      int tile = lvl->tiles[y][x];
      if (tile > 0) {
        Rectangle dst = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        if (tile == TILE_SPAWNER_CIRCLE) {
          DrawRectangleRec(dst, (Color){50, 50, 50, 255});
          DrawCircle(x * TILE_SIZE + 25, y * TILE_SIZE + 25, 18, RED);
          DrawText("C", x * TILE_SIZE + 20, y * TILE_SIZE + 15, 20, WHITE);
        } else if (tile == TILE_SPAWNER_SPIRAL) {
          DrawRectangleRec(dst, (Color){50, 50, 50, 255});
          DrawCircle(x * TILE_SIZE + 25, y * TILE_SIZE + 25, 18, PURPLE);
          DrawText("S", x * TILE_SIZE + 20, y * TILE_SIZE + 15, 20, WHITE);
        } else if (tile == TILE_SPAWNER_WAVE) {
          DrawRectangleRec(dst, (Color){50, 50, 50, 255});
          DrawCircle(x * TILE_SIZE + 25, y * TILE_SIZE + 25, 18, BLUE);
          DrawText("W", x * TILE_SIZE + 19, y * TILE_SIZE + 15, 20, WHITE);
        } else if (tile == TILE_SPAWNER_BURST) {
          DrawRectangleRec(dst, (Color){50, 50, 50, 255});
          DrawCircle(x * TILE_SIZE + 25, y * TILE_SIZE + 25, 18, ORANGE);
          DrawText("B", x * TILE_SIZE + 20, y * TILE_SIZE + 15, 20, WHITE);
        } else if (tile == TILE_CHECKPOINT) {
          DrawRectangleRec(dst, SKYBLUE);
          DrawText("CP", x * TILE_SIZE + 12, y * TILE_SIZE + 18, 18, WHITE);
        } else if (tile == TILE_DAMAGE) {
          DrawRectangleRec(dst, ORANGE);
        } else if (tile == TILE_SPIKE) {
          DrawRectangleRec(dst, RED);
          DrawTriangle(
              (Vector2){x * TILE_SIZE, y * TILE_SIZE + TILE_SIZE},
              (Vector2){x * TILE_SIZE + TILE_SIZE / 2, y * TILE_SIZE},
              (Vector2){x * TILE_SIZE + TILE_SIZE, y * TILE_SIZE + TILE_SIZE},
              DARKGRAY);
        } else {
          Rectangle src = Assets_GetTileSource(tile);
          DrawTexturePro(assets->tileset, src, dst, (Vector2){0, 0}, 0, WHITE);
        }
      }
    }
  }
}
bool Level_IsSolid(const Level *lvl, int tx, int ty) {
  if (tx < 0 || ty < 0 || tx >= lvl->width || ty >= lvl->height)
    return true;
  int tile = lvl->tiles[ty][tx];
  return (tile == TILE_GRASS || tile == TILE_DIRT || tile == TILE_STONE ||
          tile == TILE_JUMP_BOOST || tile == TILE_DAMAGE ||
          tile == TILE_SPIKE || tile == TILE_CHECKPOINT);
}
int Level_GetTile(const Level *lvl, int tx, int ty) {
  if (tx < 0 || ty < 0 || tx >= lvl->width || ty >= lvl->height)
    return 0;
  return lvl->tiles[ty][tx];
}
void Level_SetTile(Level *lvl, int tx, int ty, int tileType) {
  if (tx < 0 || ty < 0 || tx >= lvl->width || ty >= lvl->height)
    return;
  lvl->tiles[ty][tx] = tileType;
  if (tileType == TILE_GOAL) {
    lvl->goalPos = (Vector2){tx * TILE_SIZE, ty * TILE_SIZE};
    lvl->hasGoal = true;
  }
}
Rectangle Level_GetTileBounds(int tx, int ty) {
  return (Rectangle){tx * TILE_SIZE, ty * TILE_SIZE, TILE_SIZE, TILE_SIZE};
}
TileEffect Level_GetTileEffect(int tileType) {
  TileEffect effect = {false, 0, false, 1.0f, false, false};
  switch (tileType) {
  case TILE_DAMAGE:
    effect.hasDamage = true;
    effect.damageAmount = 1;
    break;
  case TILE_SPIKE:
    effect.hasDamage = true;
    effect.damageAmount = 1;
    effect.isDeadly = true;
    break;
  case TILE_JUMP_BOOST:
    effect.hasJumpBoost = true;
    effect.jumpBoostMultiplier = 1.5f;
    break;
  case TILE_CHECKPOINT:
    effect.isCheckpoint = true;
    break;
  default:
    break;
  }
  return effect;
}
