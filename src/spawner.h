#ifndef SPAWNER_H
#define SPAWNER_H
#include "config.h"
#include "raylib.h"
#define MAX_BULLETS 500
#define MAX_SPAWNERS 50
typedef struct
{
	float cooldown;
	int bulletCount;
	float bulletSpeed;
	float spreadAngle;
	float rotationSpeed;
	bool randomizeSpeed;
	float speedVariation;
	Color bulletColor;
	float bulletSize;
} SpawnerConfig;
void Spawner_Init(BulletSpawner *spawner, Vector2 position,
                  SpawnerPattern pattern);
void Spawner_InitWithConfig(BulletSpawner *spawner, Vector2 position,
                            SpawnerPattern pattern, SpawnerConfig config);
void Spawner_Update(BulletSpawner *spawner, Bullet bullets[], int *bulletCount,
                    Vector2 playerPos, float dt);
void Spawner_Draw(const BulletSpawner *spawner);
void Bullet_Update(Bullet bullets[], int *bulletCount, float dt);
void Bullet_Draw(const Bullet bullets[], int bulletCount);
bool Bullet_CheckCollision(const Bullet *bullet, Rectangle playerBounds);
SpawnerConfig Spawner_GetDefaultConfig(SpawnerPattern pattern);
void Spawner_PatternCircle(BulletSpawner *spawner, Bullet bullets[],
                           int *bulletCount);
void Spawner_PatternSpiral(BulletSpawner *spawner, Bullet bullets[],
                           int *bulletCount);
void Spawner_PatternWave(BulletSpawner *spawner, Bullet bullets[],
                         int *bulletCount);
void Spawner_PatternBurst(BulletSpawner *spawner, Bullet bullets[],
                          int *bulletCount);
void Spawner_PatternTargeting(BulletSpawner *spawner, Bullet bullets[],
                              int *bulletCount, Vector2 playerPos);
#endif
