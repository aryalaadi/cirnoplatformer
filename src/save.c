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

#include "save.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef _WIN32
#include <direct.h>
#define mkdir(dir, mode) _mkdir(dir)
#endif
void Save_Write(const GameData *data, const char *filename)
{
	mkdir("saves", 0777);
	char filepath[512];
	snprintf(filepath, sizeof(filepath), "saves/%s", filename);
	FILE *f = fopen(filepath, "wb");
	if (f)
	{
		fwrite(data, sizeof(GameData), 1, f);
		fclose(f);
	}
}
bool Save_Read(GameData *data, const char *filename)
{
	char filepath[512];
	snprintf(filepath, sizeof(filepath), "saves/%s", filename);
	FILE *f = fopen(filepath, "rb");
	if (!f)
		return false;
	size_t result = fread(data, sizeof(GameData), 1, f);
	fclose(f);
	return result == 1;
}
bool Save_ReadMetadata(SaveMetadata *meta, const char *filename)
{
	GameData data;
	if (!Save_Read(&data, filename))
	{
		meta->isValid = false;
		return false;
	}
	strncpy(meta->saveName, data.saveName, sizeof(meta->saveName) - 1);
	meta->saveName[sizeof(meta->saveName) - 1] = '\0';
	meta->currentLevel = data.currentLevel;
	meta->deathCount = data.deathCount;
	meta->health = data.health;
	meta->totalScore = data.totalScore;
	meta->currentLevelScore = data.currentLevelScore;
	meta->healthPoints = data.healthPoints;
	meta->canSpellCard = data.canSpellCard;
	meta->isValid = data.isValid;
	return true;
}
void Save_GetAllSaveFiles(SaveMetadata *saves, int maxSaves, int *count)
{
	*count = 0;
	DIR *dir = opendir("saves");
	if (!dir)
	{
		mkdir("saves", 0777);
		return;
	}
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL && *count < maxSaves)
	{
		const char *name = entry->d_name;
		size_t len = strlen(name);
		if (len > 4 && strcmp(name + len - 4, ".sav") == 0)
		{
			if (Save_ReadMetadata(&saves[*count], name))
			{
				(*count)++;
			}
		}
	}
	closedir(dir);
}
void Save_DeleteSave(const char *filename)
{
	char filepath[512];
	snprintf(filepath, sizeof(filepath), "saves/%s", filename);
	remove(filepath);
}
