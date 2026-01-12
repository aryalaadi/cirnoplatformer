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

// Where it all begins
#include "config.h"
#include "game.h"
#include "raylib.h"
int main(void)
{
	// Think of a better name for the game.
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Cirno's Hardest Parkour");
	SetTargetFPS(60);
	SetExitKey(KEY_NULL);
	Game_Init();
	while (!WindowShouldClose())
	{
		Game_Update();
		BeginDrawing();
		ClearBackground((Color){135, 206, 235, 255});
		Game_Draw();
		EndDrawing();
	}
	Game_Cleanup();
	CloseWindow();
	return 0;
}
