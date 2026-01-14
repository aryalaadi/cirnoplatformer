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

#include "editor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Comparison function for sorting level filenames
static int CompareFilenames(const void *a, const void *b)
{
	const char *str_a = (const char *)a;
	const char *str_b = (const char *)b;
	return strcmp(str_a, str_b);
}

void Editor_RefreshLevelList(LevelEditor *editor)
{
	FilePathList files = LoadDirectoryFiles("assets/levels");
	editor->totalLevelFiles = 0;
	
	// Collect all .lvl files
	for (unsigned int i = 0;
	     i < files.count && editor->totalLevelFiles < MAX_LEVELS; i++)
	{
		const char *ext = GetFileExtension(files.paths[i]);
		if (TextIsEqual(ext, ".lvl"))
		{
			strncpy(editor->levelFiles[editor->totalLevelFiles], files.paths[i],
			        255);
			editor->levelFiles[editor->totalLevelFiles][255] = '\0';
			editor->totalLevelFiles++;
		}
	}
	UnloadDirectoryFiles(files);
	
	// Sort level files alphabetically
	if (editor->totalLevelFiles > 0)
	{
		qsort(editor->levelFiles, editor->totalLevelFiles, 
		      sizeof(editor->levelFiles[0]), CompareFilenames);
	}
	
	editor->needsRefresh = false;
}

void Editor_NewLevel(LevelEditor *editor)
{
	Level_Unload(&editor->level);
	Level_Create(&editor->level, 30, 20);
	strcpy(editor->levelName, "newlevel");
	strcpy(editor->levelNameBuffer, "newlevel");
	editor->currentLevelIndex = -1;
	strcpy(editor->statusMessage, "Created new level");
	editor->statusTimer = EDITOR_STATUS_MESSAGE_DURATION;
}

void Editor_SaveLevel(LevelEditor *editor)
{
	char filepath[512];
	snprintf(filepath, sizeof(filepath), "assets/levels/%s.lvl",
	         editor->levelName);
	Level_SaveToFile(&editor->level, filepath);
	snprintf(editor->statusMessage, sizeof(editor->statusMessage), "Saved: %s",
	         editor->levelName);
	editor->statusTimer = EDITOR_STATUS_MESSAGE_DURATION;
	editor->needsRefresh = true;
}

void Editor_LoadLevel(LevelEditor *editor, int index)
{
	if (index < 0 || index >= editor->totalLevelFiles)
		return;
	Level_Unload(&editor->level);
	Level_LoadFromFile(&editor->level, editor->levelFiles[index]);
	const char *fname = GetFileNameWithoutExt(editor->levelFiles[index]);
	strncpy(editor->levelName, fname, 255);
	editor->levelName[255] = '\0';
	strncpy(editor->levelNameBuffer, fname, 255);
	editor->levelNameBuffer[255] = '\0';
	editor->currentLevelIndex = index;
	snprintf(editor->statusMessage, sizeof(editor->statusMessage), "Loaded: %s",
	         editor->levelName);
	editor->statusTimer = EDITOR_STATUS_MESSAGE_DURATION;
}
