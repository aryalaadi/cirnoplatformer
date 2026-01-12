#ifndef SAVE_H
#define SAVE_H
#include "game.h"
typedef struct
{
	char saveName[64];
	int currentLevel;
	int deathCount;
	int health;
	bool isValid;
} SaveMetadata;
void Save_Write(const GameData *data, const char *filename);
bool Save_Read(GameData *data, const char *filename);
bool Save_ReadMetadata(SaveMetadata *meta, const char *filename);
void Save_GetAllSaveFiles(SaveMetadata *saves, int maxSaves, int *count);
void Save_DeleteSave(const char *filename);
#endif
