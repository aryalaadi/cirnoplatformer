#ifndef ACHIEVEMENT_H
#define ACHIEVEMENT_H
#include "config.h"
#include <stdbool.h>
#define MAX_ACHIEVEMENTS 19
typedef enum
{
	ACH_COMPLETE_STAGE_1,
	ACH_COMPLETE_STAGE_2,
	ACH_COMPLETE_STAGE_3,
	ACH_COMPLETE_STAGE_4,
	ACH_COMPLETE_STAGE_5,
	ACH_COMPLETE_STAGE_6,
	ACH_COMPLETE_STAGE_7,
	ACH_COMPLETE_STAGE_8,
	ACH_COMPLETE_STAGE_9,
	ACH_PERFECT_STAGE_1,
	ACH_PERFECT_STAGE_2,
	ACH_PERFECT_STAGE_3,
	ACH_PERFECT_STAGE_4,
	ACH_PERFECT_STAGE_5,
	ACH_PERFECT_STAGE_6,
	ACH_PERFECT_STAGE_7,
	ACH_PERFECT_STAGE_8,
	ACH_PERFECT_STAGE_9,
	ACH_PERFECT_GAME
} AchievementID;
typedef struct
{
	AchievementID id;
	const char *name;
	const char *description;
	bool unlocked;
	int progress;
	int target;
} Achievement;
typedef struct
{
	Achievement achievements[MAX_ACHIEVEMENTS];
	int unlockedCount;
	bool newUnlock;
	AchievementID lastUnlocked;
} AchievementSystem;
void Achievement_Init(AchievementSystem *sys);
void Achievement_Load(AchievementSystem *sys);
void Achievement_Save(const AchievementSystem *sys);
bool Achievement_Unlock(AchievementSystem *sys, AchievementID id);
void Achievement_FixPointers(AchievementSystem *sys);
bool Achievement_IsUnlocked(const AchievementSystem *sys, AchievementID id);
const Achievement *Achievement_Get(const AchievementSystem *sys,
                                   AchievementID id);
float Achievement_GetPercentage(const AchievementSystem *sys);
void Achievement_CheckStageComplete(AchievementSystem *sys, int stage,
                                    int deaths);
void Achievement_CheckPerfectGame(AchievementSystem *sys,
                                  const int *levelDeaths, int levelCount);
#endif
