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

#ifndef EDITOR_H
#define EDITOR_H

#include "assets.h"
#include "config.h"
#include "level.h"
#include "raylib.h"

typedef struct
{
	Level level;
	Assets assets;
	Camera2D camera;
	int selectedTile;
	char levelName[256];
	char levelNameBuffer[256];
	int levelWidth;
	int levelHeight;
	int currentLevelIndex;
	int totalLevelFiles;
	char levelFiles[MAX_LEVELS][256];
	bool uiInteracting;
	bool needsRefresh;
	char statusMessage[256];
	float statusTimer;
	bool editingBackground;
	bool vnEditorOpen;
	int selectedDialogue;
	char vnCharNameBuffer[32];
	char vnTextBuffer[256];
	char vnSpriteBuffer[64];
	int vnBgColorIndex;
	bool vnEditingCharName;
	bool vnEditingText;
	bool vnEditingSprite;
	float vnScrollOffset;
	float tileScrollOffset;
} LevelEditor;

typedef enum
{
	SAVE_MENU_NONE,
	SAVE_MENU_SAVE,
	SAVE_MENU_LOAD
} SaveMenuState;

// Initialize the level editor
void Editor_Init(LevelEditor *editor);

// Refresh the list of available level files
void Editor_RefreshLevelList(LevelEditor *editor);

// Create a new blank level
void Editor_NewLevel(LevelEditor *editor);

// Save the current level to file
void Editor_SaveLevel(LevelEditor *editor);

// Load a level from the file list
void Editor_LoadLevel(LevelEditor *editor, int index);

// Update editor state (input handling, camera, etc.)
void Editor_Update(LevelEditor *editor, float dt);

// Draw the level editor
void Editor_Draw(const LevelEditor *editor);

// Cleanup editor resources
void Editor_Cleanup(LevelEditor *editor);

// Get the editor pause selection
int Editor_GetPauseSelection(void);

// Set the editor pause selection
void Editor_SetPauseSelection(int selection);

#endif
