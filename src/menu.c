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

#include "menu.h"
#include "achievement.h"
#include "game.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>

// Everything in the menu is implemented here.
typedef enum
{
	MENU_MAIN,
	MENU_SETTINGS,
	MENU_KEYBINDINGS,
	MENU_ACHIEVEMENTS
} MenuScreen;

static int selectedIndex;
static MenuScreen currentScreen;
static Settings settings;
static int waitingForKey = -1;
static int achievementScroll = 0;
static const char *mainMenuItems[] = {"New Game",     "Load Game",
                                      "Achievements", "Level Editor",
                                      "Settings",     "Quit"};

static const char *settingsItems[] = {
    "Resolution: ", "Fullscreen: ", "Sound: ", "Key Bindings", "Back"};

static const char *keybindingItems[] = {
    "Move Left: ", "Move Right: ",      "Jump: ",
    "Dash: ",      "Wall Cling: ",      "Float: ", 
    "Pause: ",     "Slow Down: ",       "Heal: ",
    "Reset to Defaults", "Back"};

static const char *resolutionNames[] = {"800x600", "1024x768", "1280x720",
                                        "1920x1080"};

void Menu_SetDefaultKeyBindings(void)
{
	settings.keys.moveLeft = KEY_A;
	settings.keys.moveRight = KEY_D;
	settings.keys.jump = KEY_SPACE;
	settings.keys.dash = KEY_LEFT_SHIFT;
	settings.keys.floatKey = KEY_F;
	settings.keys.wallCling = KEY_E;
	settings.keys.pause = KEY_ESCAPE;
	settings.keys.menuUp = KEY_UP;
	settings.keys.menuDown = KEY_DOWN;
	settings.keys.menuSelect = KEY_ENTER;
	settings.keys.menuBack = KEY_ESCAPE;
	settings.keys.slowDown = KEY_S;
	settings.keys.heal = KEY_H;
}

// TODO: support mouse button because my friend uses it for some reason.
const char *Menu_GetKeyName(int key)
{
	switch (key)
	{
	case KEY_SPACE:
		return "SPACE";
	case KEY_ESCAPE:
		return "ESC";
	case KEY_ENTER:
		return "ENTER";
	case KEY_TAB:
		return "TAB";
	case KEY_BACKSPACE:
		return "BACKSPACE";
	case KEY_RIGHT:
		return "RIGHT";
	case KEY_LEFT:
		return "LEFT";
	case KEY_DOWN:
		return "DOWN";
	case KEY_UP:
		return "UP";
	case KEY_LEFT_SHIFT:
		return "L-SHIFT";
	case KEY_LEFT_CONTROL:
		return "L-CTRL";
	case KEY_LEFT_ALT:
		return "L-ALT";
	case KEY_RIGHT_SHIFT:
		return "R-SHIFT";
	case KEY_RIGHT_CONTROL:
		return "R-CTRL";
	case KEY_RIGHT_ALT:
		return "R-ALT";
	default:
		if (key >= KEY_A && key <= KEY_Z)
		{
			static char keyStr[2] = {0};
			keyStr[0] = 'A' + (key - KEY_A);
			return keyStr;
		}
		if (key >= KEY_ZERO && key <= KEY_NINE)
		{
			static char keyStr[2] = {0};
			keyStr[0] = '0' + (key - KEY_ZERO);
			return keyStr;
		}
		return "???";
	}
}

void Menu_ApplyResolution(void)
{
	int width, height;
	switch (settings.resolution)
	{
	case RES_800x600:
		width = 800;
		height = 600;
		break;
	case RES_1024x768:
		width = 1024;
		height = 768;
		break;
	case RES_1280x720:
		width = 1280;
		height = 720;
		break;
	case RES_1920x1080:
		width = 1920;
		height = 1080;
		break;
	default:
		width = 800;
		height = 600;
		break;
	}
	SetWindowSize(width, height);
}

void Menu_Init(void)
{
	selectedIndex = 0;
	currentScreen = MENU_MAIN;
	waitingForKey = -1;
	settings.fullscreen = false;
	settings.soundEnabled = true;
	settings.masterVolume = 0.5f;
	settings.resolution = RES_800x600;
	Menu_SetDefaultKeyBindings();
	Menu_LoadSettings();
}
Settings *Menu_GetSettings(void) { return &settings; }
void Menu_SaveSettings(void)
{
	FILE *f = fopen("settings.cfg", "wb");
	if (f)
	{
		fwrite(&settings, sizeof(Settings), 1, f);
		fclose(f);
	}
}
void Menu_LoadSettings(void)
{
	FILE *f = fopen("settings.cfg", "rb");
	if (f)
	{
		Settings loadedSettings;
		size_t result = fread(&loadedSettings, sizeof(Settings), 1, f);
		fclose(f);
		if (result == 1)
		{
			settings = loadedSettings;
		}
		else
		{
			Menu_SetDefaultKeyBindings();
		}
		Menu_ApplyResolution();
		if (settings.fullscreen && !IsWindowFullscreen())
		{
			ToggleFullscreen();
		}
		else if (!settings.fullscreen && IsWindowFullscreen())
		{
			ToggleFullscreen();
		}
		SetMasterVolume(settings.soundEnabled ? settings.masterVolume : 0.0f);
	}
	else
	{
		Menu_SetDefaultKeyBindings();
	}
}
void Menu_Update(void)
{
	if (waitingForKey >= 0)
	{
		int key = GetKeyPressed();
		if (key != 0 && key != KEY_ESCAPE)
		{
			switch (waitingForKey)
			{
			case 0:
				settings.keys.moveLeft = key;
				break;
			case 1:
				settings.keys.moveRight = key;
				break;
			case 2:
				settings.keys.jump = key;
				break;
			case 3:
				settings.keys.dash = key;
				break;
			case 5:
				settings.keys.floatKey = key;
				break;
			case 4:
				settings.keys.wallCling = key;
				break;
			case 6:
				settings.keys.pause = key;
				break;
			case 7:
				settings.keys.slowDown = key;
				break;
			case 8:
				settings.keys.heal = key;
				break;
			}
			waitingForKey = -1;
			Menu_SaveSettings();
		}
		else if (key == KEY_ESCAPE)
		{
			waitingForKey = -1;
		}
		return;
	}
	if (currentScreen == MENU_MAIN)
	{
		if (IsKeyPressed(settings.keys.menuDown) || IsKeyPressed(KEY_DOWN))
			selectedIndex = (selectedIndex + 1) % 6;
		if (IsKeyPressed(settings.keys.menuUp) || IsKeyPressed(KEY_UP))
			selectedIndex = (selectedIndex + 5) % 6;
		if (IsKeyPressed(settings.keys.menuSelect) || IsKeyPressed(KEY_ENTER))
		{
			if (selectedIndex == 0)
				Game_StartNew();
			if (selectedIndex == 1)
				Game_LoadSave();
			if (selectedIndex == 2)
			{
				currentScreen = MENU_ACHIEVEMENTS;
				selectedIndex = 0;
				achievementScroll = 0;
			}
			if (selectedIndex == 3)
				Game_StartLevelEditor();
			if (selectedIndex == 4)
			{
				currentScreen = MENU_SETTINGS;
				selectedIndex = 0;
			}
			if (selectedIndex == 5)
				CloseWindow();
		}
	}
	else if (currentScreen == MENU_SETTINGS)
	{
		if (IsKeyPressed(KEY_DOWN))
			selectedIndex = (selectedIndex + 1) % 5;
		if (IsKeyPressed(KEY_UP))
			selectedIndex = (selectedIndex + 4) % 5;
		if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
		{
			if (selectedIndex == 0)
			{
				settings.resolution = (settings.resolution + 1) % RES_COUNT;
				Menu_ApplyResolution();
				Menu_SaveSettings();
			}
			if (selectedIndex == 1)
			{
				settings.fullscreen = !settings.fullscreen;
				ToggleFullscreen();
				Menu_SaveSettings();
			}
			if (selectedIndex == 2)
			{
				settings.soundEnabled = !settings.soundEnabled;
				SetMasterVolume(settings.soundEnabled ? settings.masterVolume
				                                      : 0.0f);
				Menu_SaveSettings();
			}
			if (selectedIndex == 3)
			{
				currentScreen = MENU_KEYBINDINGS;
				selectedIndex = 0;
			}
			if (selectedIndex == 4)
			{
				currentScreen = MENU_MAIN;
				selectedIndex = 0;
			}
		}
		if (IsKeyPressed(KEY_ESCAPE))
		{
			currentScreen = MENU_MAIN;
			selectedIndex = 0;
		}
	}
	else if (currentScreen == MENU_ACHIEVEMENTS)
	{
		int scrollDelta = GetMouseWheelMove();
		if (scrollDelta != 0)
		{
			achievementScroll -= scrollDelta * 50;
		}
		if (IsKeyPressed(KEY_DOWN))
			achievementScroll += 50;
		if (IsKeyPressed(KEY_UP))
			achievementScroll -= 50;
		if (achievementScroll < 0)
			achievementScroll = 0;
		int maxScroll = MAX_ACHIEVEMENTS * 90 - 400;
		if (maxScroll < 0)
			maxScroll = 0;
		if (achievementScroll > maxScroll)
			achievementScroll = maxScroll;
		if (IsKeyPressed(KEY_ESCAPE))
		{
			currentScreen = MENU_MAIN;
			selectedIndex = 0;
		}
	}
	else if (currentScreen == MENU_KEYBINDINGS)
	{
		if (IsKeyPressed(KEY_DOWN))
			selectedIndex = (selectedIndex + 1) % 11;
		if (IsKeyPressed(KEY_UP))
			selectedIndex = (selectedIndex + 10) % 11;
		if (IsKeyPressed(KEY_ENTER))
		{
			if (selectedIndex < 9)
			{
				waitingForKey = selectedIndex;
			}
			else if (selectedIndex == 9)
			{
				Menu_SetDefaultKeyBindings();
				Menu_SaveSettings();
			}
			else if (selectedIndex == 10)
			{
				currentScreen = MENU_SETTINGS;
				selectedIndex = 0;
			}
		}
		if (IsKeyPressed(KEY_ESCAPE))
		{
			currentScreen = MENU_SETTINGS;
			selectedIndex = 0;
		}
	}
}
void Menu_Draw(void)
{
	DrawText("CIRNO'S HARDEST PARKOUR", SCREEN_WIDTH / 2 - 200, 80, 30,
	         SKYBLUE);
	if (currentScreen == MENU_MAIN)
	{
		for (int i = 0; i < 6; i++)
		{
			Color c = (i == selectedIndex) ? YELLOW : WHITE;
			DrawText(mainMenuItems[i], SCREEN_WIDTH / 2 - 80, 200 + i * 40, 28,
			         c);
		}
		DrawText("Use customizable controls - check Settings!",
		         SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT - 80, 18, GRAY);
		DrawText("Press ESC/P to Pause during gameplay", SCREEN_WIDTH / 2 - 180,
		         SCREEN_HEIGHT - 50, 18, GRAY);
	}
	else if (currentScreen == MENU_SETTINGS)
	{
		DrawText("SETTINGS", SCREEN_WIDTH / 2 - 80, 150, 32, SKYBLUE);
		for (int i = 0; i < 5; i++)
		{
			Color c = (i == selectedIndex) ? YELLOW : WHITE;
			if (i == 0)
			{
				DrawText(settingsItems[i], SCREEN_WIDTH / 2 - 140, 220 + i * 40,
				         24, c);
				DrawText(resolutionNames[settings.resolution],
				         SCREEN_WIDTH / 2 + 80, 220 + i * 40, 24, c);
			}
			else if (i == 1)
			{
				DrawText(settingsItems[i], SCREEN_WIDTH / 2 - 140, 220 + i * 40,
				         24, c);
				DrawText(settings.fullscreen ? "ON" : "OFF",
				         SCREEN_WIDTH / 2 + 80, 220 + i * 40, 24, c);
			}
			else if (i == 2)
			{
				DrawText(settingsItems[i], SCREEN_WIDTH / 2 - 140, 220 + i * 40,
				         24, c);
				DrawText(settings.soundEnabled ? "ON" : "OFF",
				         SCREEN_WIDTH / 2 + 80, 220 + i * 40, 24, c);
			}
			else
			{
				DrawText(settingsItems[i], SCREEN_WIDTH / 2 - 140, 220 + i * 40,
				         24, c);
			}
		}
		DrawText("Press ENTER to toggle/select", SCREEN_WIDTH / 2 - 110, 440,
		         18, GRAY);
		DrawText("Press ESC to go back", SCREEN_WIDTH / 2 - 90, 470, 18, GRAY);
	}
	else if (currentScreen == MENU_ACHIEVEMENTS)
	{
		DrawText("ACHIEVEMENTS", SCREEN_WIDTH / 2 - 120, 50, 32, SKYBLUE);
		const AchievementSystem *sys = Game_GetAchievementSystem();
		if (sys)
		{
			char buffer[256];
			snprintf(buffer, sizeof(buffer), "Unlocked: %d / %d (%.1f%%)",
			         sys->unlockedCount, MAX_ACHIEVEMENTS,
			         Achievement_GetPercentage(sys));
			DrawText(buffer, SCREEN_WIDTH / 2 - 120, 95, 20, YELLOW);
			int listX = 40;
			int listY = 140;
			int listW = SCREEN_WIDTH - 80;
			int listH = SCREEN_HEIGHT - 200;
			DrawRectangle(listX, listY, listW, listH, (Color){20, 20, 30, 255});
			DrawRectangleLinesEx((Rectangle){listX, listY, listW, listH}, 2,
			                     SKYBLUE);
			BeginScissorMode(listX + 5, listY + 5, listW - 10, listH - 10);
			int yPos = listY + 10 - achievementScroll;
			for (int i = 0; i < MAX_ACHIEVEMENTS; i++)
			{
				const Achievement *ach = Achievement_Get(sys, i);
				if (!ach)
					continue;
				if (yPos + 80 < listY || yPos > listY + listH)
				{
					yPos += 90;
					continue;
				}
				int boxX = listX + 15;
				int boxY = yPos;
				int boxW = listW - 30;
				int boxH = 75;
				Color boxColor = ach->unlocked ? (Color){40, 60, 40, 255}
				                               : (Color){40, 40, 50, 255};
				Color borderColor = ach->unlocked ? LIME : GRAY;
				DrawRectangle(boxX, boxY, boxW, boxH, boxColor);
				DrawRectangleLinesEx((Rectangle){boxX, boxY, boxW, boxH}, 2,
				                     borderColor);
				if (ach->unlocked)
				{
					DrawText("✓", boxX + 10, boxY + 25, 30, LIME);
				}
				else
				{
					DrawText("?", boxX + 10, boxY + 25, 30, GRAY);
				}
				Color nameColor = ach->unlocked ? LIME : LIGHTGRAY;
				DrawText(ach->name, boxX + 50, boxY + 10, 20, nameColor);
				Color descColor = ach->unlocked ? WHITE : DARKGRAY;
				DrawText(ach->description, boxX + 50, boxY + 38, 16, descColor);
				yPos += 90;
			}
			EndScissorMode();
			if (MAX_ACHIEVEMENTS * 90 > listH)
			{
				int scrollbarH = listH - 20;
				int thumbH = (listH * scrollbarH) / (MAX_ACHIEVEMENTS * 90);
				if (thumbH < 30)
					thumbH = 30;
				int maxScroll = MAX_ACHIEVEMENTS * 90 - listH;
				if (maxScroll < 1)
					maxScroll = 1;
				int thumbY =
				    listY + 10 +
				    ((achievementScroll * (scrollbarH - thumbH)) / maxScroll);
				DrawRectangle(listX + listW - 15, listY + 10, 8, scrollbarH,
				              (Color){60, 60, 70, 255});
				DrawRectangle(listX + listW - 15, thumbY, 8, thumbH, SKYBLUE);
			}
		}
		else
		{
			DrawText("No save file loaded", SCREEN_WIDTH / 2 - 100,
			         SCREEN_HEIGHT / 2, 24, RED);
		}
		DrawText(
		    "Use mouse wheel or arrow keys to scroll | Press ESC to go back",
		    50, SCREEN_HEIGHT - 40, 16, GRAY);
	}
	else if (currentScreen == MENU_KEYBINDINGS)
	{
		DrawText("KEY BINDINGS", SCREEN_WIDTH / 2 - 110, 100, 32, SKYBLUE);
		if (waitingForKey >= 0)
		{
			DrawText("Press any key...", SCREEN_WIDTH / 2 - 90, 150, 24,
			         YELLOW);
			DrawText("(ESC to cancel)", SCREEN_WIDTH / 2 - 80, 180, 18, GRAY);
		}
		for (int i = 0; i < 11; i++)
		{
			Color c =
			    (i == selectedIndex && waitingForKey < 0) ? YELLOW : WHITE;
			int yPos = 220 + i * 32;
			if (i < 9)
			{
				DrawText(keybindingItems[i], SCREEN_WIDTH / 2 - 180, yPos, 20,
				         c);
				const char *keyName = "";
				switch (i)
				{
				case 0:
					keyName = Menu_GetKeyName(settings.keys.moveLeft);
					break;
				case 1:
					keyName = Menu_GetKeyName(settings.keys.moveRight);
					break;
				case 2:
					keyName = Menu_GetKeyName(settings.keys.jump);
					break;
				case 3:
					keyName = Menu_GetKeyName(settings.keys.dash);
					break;
				case 4:
					keyName = Menu_GetKeyName(settings.keys.wallCling);
					break;
				case 5:
					keyName = Menu_GetKeyName(settings.keys.floatKey);
					break;
				case 6:
					keyName = Menu_GetKeyName(settings.keys.pause);
					break;
				case 7:
					keyName = Menu_GetKeyName(settings.keys.slowDown);
					break;
				case 8:
					keyName = Menu_GetKeyName(settings.keys.heal);
					break;
				}
				if (waitingForKey == i)
				{
					DrawText("???", SCREEN_WIDTH / 2 + 50, yPos, 20, YELLOW);
				}
				else
				{
					DrawText(keyName, SCREEN_WIDTH / 2 + 50, yPos, 20, c);
				}
			}
			else
			{
				DrawText(keybindingItems[i], SCREEN_WIDTH / 2 - 100, yPos, 20,
				         c);
			}
		}
		DrawText("Press ENTER to rebind", SCREEN_WIDTH / 2 - 100, 530, 18,
		         GRAY);
		DrawText("Press ESC to go back", SCREEN_WIDTH / 2 - 95, 555, 18, GRAY);
	}
}
