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

#include "achievement.h"
#include <stdio.h>
#include <string.h>
static const char *achievementNames[MAX_ACHIEVEMENTS] = {
    "Stage 1 Complete", "Stage 2 Complete", "Stage 3 Complete",
    "Stage 4 Complete", "Stage 5 Complete", "Stage 6 Complete",
    "Stage 7 Complete", "Stage 8 Complete", "Stage 9 Complete",
    "Stage 1 Perfect",  "Stage 2 Perfect",  "Stage 3 Perfect",
    "Stage 4 Perfect",  "Stage 5 Perfect",  "Stage 6 Perfect",
    "Stage 7 Perfect",  "Stage 8 Perfect",  "Stage 9 Perfect",
    "Perfect Game"};
static const char *achievementDescriptions[MAX_ACHIEVEMENTS] = {
    "Complete Stage 1",
    "Complete Stage 2",
    "Complete Stage 3",
    "Complete Stage 4",
    "Complete Stage 5",
    "Complete Stage 6",
    "Complete Stage 7",
    "Complete Stage 8",
    "Complete Stage 9",
    "Complete Stage 1 without dying",
    "Complete Stage 2 without dying",
    "Complete Stage 3 without dying",
    "Complete Stage 4 without dying",
    "Complete Stage 5 without dying",
    "Complete Stage 6 without dying",
    "Complete Stage 7 without dying",
    "Complete Stage 8 without dying",
    "Complete Stage 9 without dying",
    "Complete the entire game without dying"};
void Achievement_Init(AchievementSystem *sys)
{
	sys->unlockedCount = 0;
	sys->newUnlock = false;
	sys->lastUnlocked = ACH_COMPLETE_STAGE_1;
	for (int i = 0; i < MAX_ACHIEVEMENTS; i++)
	{
		sys->achievements[i].id = (AchievementID)i;
		sys->achievements[i].name = achievementNames[i];
		sys->achievements[i].description = achievementDescriptions[i];
		sys->achievements[i].unlocked = false;
		sys->achievements[i].progress = 0;
		sys->achievements[i].target = 1;
	}
}
void Achievement_FixPointers(AchievementSystem *sys)
{
	for (int i = 0; i < MAX_ACHIEVEMENTS; i++)
	{
		sys->achievements[i].name = achievementNames[i];
		sys->achievements[i].description = achievementDescriptions[i];
	}
}
void Achievement_Load(AchievementSystem *sys)
{
	const char *filepath = "achievements.dat";
	FILE *f = fopen(filepath, "rb");
	if (f)
	{
		for (int i = 0; i < MAX_ACHIEVEMENTS; i++)
		{
			fread(&sys->achievements[i].unlocked, sizeof(bool), 1, f);
		}
		fclose(f);
		sys->unlockedCount = 0;
		for (int i = 0; i < MAX_ACHIEVEMENTS; i++)
		{
			if (sys->achievements[i].unlocked)
			{
				sys->unlockedCount++;
			}
		}
	}
}
void Achievement_Save(const AchievementSystem *sys)
{
	const char *filepath = "achievements.dat";
	FILE *f = fopen(filepath, "wb");
	if (f)
	{
		for (int i = 0; i < MAX_ACHIEVEMENTS; i++)
		{
			fwrite(&sys->achievements[i].unlocked, sizeof(bool), 1, f);
		}
		fclose(f);
	}
}
bool Achievement_Unlock(AchievementSystem *sys, AchievementID id)
{
	if (id < 0 || id >= MAX_ACHIEVEMENTS)
		return false;
	if (!sys->achievements[id].unlocked)
	{
		sys->achievements[id].unlocked = true;
		sys->unlockedCount++;
		sys->newUnlock = true;
		sys->lastUnlocked = id;
		return true;
	}
	return false;
}
bool Achievement_IsUnlocked(const AchievementSystem *sys, AchievementID id)
{
	if (id < 0 || id >= MAX_ACHIEVEMENTS)
		return false;
	return sys->achievements[id].unlocked;
}
const Achievement *Achievement_Get(const AchievementSystem *sys,
                                   AchievementID id)
{
	if (id < 0 || id >= MAX_ACHIEVEMENTS)
		return NULL;
	return &sys->achievements[id];
}
float Achievement_GetPercentage(const AchievementSystem *sys)
{
	return (float)sys->unlockedCount / (float)MAX_ACHIEVEMENTS * 100.0f;
}
void Achievement_CheckStageComplete(AchievementSystem *sys, int stage,
                                    int deaths)
{
	if (stage < 0 || stage >= BASE_LEVEL_COUNT)
		return;
	Achievement_Unlock(sys, (AchievementID)(ACH_COMPLETE_STAGE_1 + stage));
	if (deaths == 0)
	{
		Achievement_Unlock(sys, (AchievementID)(ACH_PERFECT_STAGE_1 + stage));
	}
}
void Achievement_CheckPerfectGame(AchievementSystem *sys,
                                  const int *levelDeaths, int levelCount)
{
	if (levelCount != BASE_LEVEL_COUNT)
		return;
	bool perfectGame = true;
	for (int i = 0; i < levelCount; i++)
	{
		if (levelDeaths[i] > 0)
		{
			perfectGame = false;
			break;
		}
	}
	if (perfectGame)
	{
		Achievement_Unlock(sys, ACH_PERFECT_GAME);
	}
}
