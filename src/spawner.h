#ifndef SPAWNER_H
#define SPAWNER_H
#include "config.h"
#include "raylib.h"
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

// Spawner functions
void Spawner_Init(BulletSpawner *spawner, Vector2 position,
                  SpawnerPattern pattern);
void Spawner_InitWithConfig(BulletSpawner *spawner, Vector2 position,
                            SpawnerPattern pattern, SpawnerConfig config);
void Spawner_Update(BulletSpawner *spawner, Bullet bullets[], int *bulletCount,
                    Collectible collectibles[], int *collectibleCount,
                    Vector2 playerPos, float dt);
void Spawner_Draw(const BulletSpawner *spawner);

// Bullet functions
void Bullet_Update(Bullet bullets[], int *bulletCount, float dt);
void Bullet_Draw(const Bullet bullets[], int bulletCount);
bool Bullet_CheckCollision(const Bullet *bullet, Rectangle playerBounds);

// Collectible functions
void Collectible_Update(Collectible collectibles[], int *collectibleCount, float dt);
void Collectible_Draw(const Collectible collectibles[], int collectibleCount);
bool Collectible_CheckCollection(const Collectible *collectible, Rectangle playerBounds);
void Collectible_Spawn(Collectible collectibles[], int *collectibleCount,
                       Vector2 position, CollectibleType type);

// Parry effect functions
void ParryEffect_Spawn(ParryEffect effects[], int *effectCount, Vector2 position);
void ParryEffect_Update(ParryEffect effects[], int *effectCount, float dt);
void ParryEffect_Draw(const ParryEffect effects[], int effectCount);

// Spawner damage functions
BulletSpawner* Spawner_FindNearest(BulletSpawner spawners[], int spawnerCount, Vector2 position);
void Spawner_TakeDamage(BulletSpawner *spawner, int damage);

// Config helpers
SpawnerConfig Spawner_GetDefaultConfig(SpawnerPattern pattern);

// Pattern generators
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
