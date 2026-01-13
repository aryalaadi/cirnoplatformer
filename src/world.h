#ifndef WORLD_H
#define WORLD_H
#include "assets.h"
#include "level.h"
#include "player.h"
#include "spawner.h"
typedef struct
{
	Level level;
	Player player;
	Camera2D camera;
	Assets assets;
	BulletSpawner spawners[MAX_SPAWNERS];
	int spawnerCount;
	Bullet bullets[MAX_BULLETS];
	int bulletCount;
	Collectible collectibles[MAX_COLLECTIBLES];
	int collectibleCount;
} World;
void World_Load(World *world, int levelIndex);
void World_Unload(World *world);
void World_Update(World *world, float dt, const KeyBindings *keys);
void World_Draw(const World *world);
bool World_LevelCompleted(const World *world);
bool World_IsPlayerOutOfBounds(const World *world);
void World_ResetBullets(World *world);
int World_CollectItems(World *world, int *healthPointsCollected, int *scoreCollected);
#endif
