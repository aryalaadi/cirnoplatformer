#ifndef GAME_H
#define GAME_H
#include "achievement.h"
#include "world.h"
typedef enum
{
	STATE_MENU,
	STATE_NAME_INPUT,
	STATE_LOAD_GAME,
	STATE_PLAYING,
	STATE_PAUSED,
	STATE_DEATH_SCREEN,
	STATE_LEVEL_COMPLETE,
	STATE_GAME_COMPLETE,
	STATE_CREDITS,
	STATE_VISUAL_NOVEL,
	STATE_LEVEL_EDITOR,
	STATE_EDITOR_PAUSED,
	STATE_VN_EDITOR
} GameState;
typedef struct
{
	char playerName[32];
	int currentLevel;
	int totalLevels;
	int levelsCompleted;
	int deathCount;
	int health;
	int levelDeaths[MAX_LEVELS];
	bool levelProgress[MAX_LEVELS];
	char saveName[64];
	bool isValid;
	AchievementSystem achievements;
	int totalScore;           // Total score across all completed levels
	int currentLevelScore;    // Score in current level (reset on death)
	int healthPoints;         // Collected health points (10 = 1 heal)
} GameData;
void Game_Init(void);
void Game_Update(void);
void Game_Draw(void);
void Game_StartNew(void);
void Game_LoadSave(void);
void Game_SaveProgress(void);
void Game_Cleanup(void);
void Game_PauseToggle(void);
void Game_NextLevel(void);
void Game_StartLevelEditor(void);
void Game_ShowSaveMenu(void);
void Game_ShowLoadMenu(void);
const AchievementSystem *Game_GetAchievementSystem(void);
#endif
