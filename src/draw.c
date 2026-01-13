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

#include "draw.h"

static int pauseSelection = 0;
static int editorPauseSelection = 0;

static const char *pauseMenuItems[] = {"Resume", "Save", "Main Menu"};
static const char *editorPauseMenuItems[] = {"Resume", "Save Level",
                                             "Main Menu"};

void Pause_Init(void)
{
	pauseSelection = 0;
	editorPauseSelection = 0;
}

void Pause_Update(const KeyBindings *keys)
{
	if (IsKeyPressed(keys->menuDown) || IsKeyPressed(KEY_DOWN))
		pauseSelection = (pauseSelection + 1) % PAUSE_COUNT;
	if (IsKeyPressed(keys->menuUp) || IsKeyPressed(KEY_UP))
		pauseSelection = (pauseSelection + PAUSE_COUNT - 1) % PAUSE_COUNT;
}

void Pause_Draw(void)
{
	// Semi-transparent overlay
	DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 150});

	DrawText("PAUSED", UI_PAUSE_MENU_X, UI_PAUSE_MENU_Y - 60,
	         UI_PAUSE_MENU_FONT_SIZE + 4, SKYBLUE);

	for (int i = 0; i < PAUSE_COUNT; i++)
	{
		Color c = (i == pauseSelection) ? YELLOW : WHITE;
		DrawText(pauseMenuItems[i], UI_PAUSE_MENU_X,
		         UI_PAUSE_MENU_Y + i * UI_PAUSE_MENU_ITEM_HEIGHT,
		         UI_PAUSE_MENU_FONT_SIZE, c);
	}

	DrawText("Use Arrow Keys + ENTER", SCREEN_WIDTH / 2 - 110,
	         SCREEN_HEIGHT - 60, 20, GRAY);
}

void EditorPause_Update(void)
{
	if (IsKeyPressed(KEY_DOWN))
		editorPauseSelection = (editorPauseSelection + 1) % EDITOR_PAUSE_COUNT;
	if (IsKeyPressed(KEY_UP))
		editorPauseSelection =
		    (editorPauseSelection + EDITOR_PAUSE_COUNT - 1) %
		    EDITOR_PAUSE_COUNT;
}

void EditorPause_Draw(void)
{
	// Semi-transparent overlay
	DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 150});

	DrawText("EDITOR PAUSED", UI_PAUSE_MENU_X - 30, UI_PAUSE_MENU_Y - 60,
	         UI_PAUSE_MENU_FONT_SIZE + 4, SKYBLUE);

	for (int i = 0; i < EDITOR_PAUSE_COUNT; i++)
	{
		Color c = (i == editorPauseSelection) ? YELLOW : WHITE;
		DrawText(editorPauseMenuItems[i], UI_PAUSE_MENU_X,
		         UI_PAUSE_MENU_Y + i * UI_PAUSE_MENU_ITEM_HEIGHT,
		         UI_PAUSE_MENU_FONT_SIZE, c);
	}

	DrawText("Use Arrow Keys + ENTER", SCREEN_WIDTH / 2 - 110,
	         SCREEN_HEIGHT - 60, 20, GRAY);
}

int Pause_GetSelection(void) { return pauseSelection; }

void Pause_SetSelection(int selection) { pauseSelection = selection; }

int EditorPause_GetSelection(void) { return editorPauseSelection; }

void EditorPause_SetSelection(int selection)
{
	editorPauseSelection = selection;
}
