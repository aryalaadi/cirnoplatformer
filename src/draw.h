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

#ifndef DRAW_H
#define DRAW_H

#include "config.h"
#include "raylib.h"

// Game pause menu options
typedef enum
{
	PAUSE_RESUME,
	PAUSE_SAVE,
	PAUSE_MENU,
	PAUSE_COUNT
} PauseOption;

// Editor pause menu options
typedef enum
{
	EDITOR_PAUSE_RESUME,
	EDITOR_PAUSE_SAVE,
	EDITOR_PAUSE_MENU,
	EDITOR_PAUSE_COUNT
} EditorPauseOption;

// Initialize pause menu state
void Pause_Init(void);

// Update pause menu (input handling)
void Pause_Update(const KeyBindings *keys);

// Draw pause menu
void Pause_Draw(void);

// Update editor pause menu
void EditorPause_Update(void);

// Draw editor pause menu
void EditorPause_Draw(void);

// Get current pause selection
int Pause_GetSelection(void);

// Set pause selection
void Pause_SetSelection(int selection);

// Get current editor pause selection
int EditorPause_GetSelection(void);

// Set editor pause selection
void EditorPause_SetSelection(int selection);

#endif
