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

#include "spawner.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
static Vector2 Vector2Add(Vector2 v1, Vector2 v2)
{
	return (Vector2){v1.x + v2.x, v1.y + v2.y};
}
static Vector2 Vector2Subtract(Vector2 v1, Vector2 v2)
{
	return (Vector2){v1.x - v2.x, v1.y - v2.y};
}
SpawnerConfig Spawner_GetDefaultConfig(SpawnerPattern pattern)
{
	SpawnerConfig config = {0};
	switch (pattern)
	{
	case SPAWNER_PATTERN_CIRCLE:
		config.cooldown = 3.0f;
		config.bulletCount = 4;
		config.bulletSpeed = 150.0f;
		config.spreadAngle = 360.0f;
		config.rotationSpeed = 0;
		config.randomizeSpeed = false;
		config.speedVariation = 0;
		config.bulletColor = RED;
		config.bulletSize = 4.0f;
		break;
	case SPAWNER_PATTERN_SPIRAL:
		config.cooldown = 2.0f;
		config.bulletCount = 2;
		config.bulletSpeed = 120.0f;
		config.spreadAngle = 0;
		config.rotationSpeed = 180.0f;
		config.randomizeSpeed = false;
		config.speedVariation = 0;
		config.bulletColor = PURPLE;
		config.bulletSize = 4.0f;
		break;
	case SPAWNER_PATTERN_WAVE:
		config.cooldown = 1.0f;
		config.bulletCount = 3;
		config.bulletSpeed = 100.0f;
		config.spreadAngle = 60.0f;
		config.rotationSpeed = 90.0f;
		config.randomizeSpeed = false;
		config.speedVariation = 0;
		config.bulletColor = BLUE;
		config.bulletSize = 6.0f;
		break;
	case SPAWNER_PATTERN_BURST:
		config.cooldown = 1.5f;
		config.bulletCount = 12;
		config.bulletSpeed = 180.0f;
		config.spreadAngle = 360.0f;
		config.rotationSpeed = 0;
		config.randomizeSpeed = true;
		config.speedVariation = 40.0f;
		config.bulletColor = ORANGE;
		config.bulletSize = 7.0f;
		break;
	case SPAWNER_PATTERN_TARGETING:
		config.cooldown = 1.2f;
		config.bulletCount = 9;
		config.bulletSpeed = 140.0f;
		config.spreadAngle = 45.0f;
		config.rotationSpeed = 0;
		config.randomizeSpeed = false;
		config.speedVariation = 0;
		config.bulletColor = YELLOW;
		config.bulletSize = 5.0f;
		break;
	}
	return config;
}
void Spawner_Init(BulletSpawner *spawner, Vector2 position,
                  SpawnerPattern pattern)
{
	SpawnerConfig config = Spawner_GetDefaultConfig(pattern);
	Spawner_InitWithConfig(spawner, position, pattern, config);
}
void Spawner_InitWithConfig(BulletSpawner *spawner, Vector2 position,
                            SpawnerPattern pattern, SpawnerConfig config)
{
	spawner->position = position;
	spawner->pattern = pattern;
	spawner->active = true;
	spawner->timer = 0;
	spawner->angleOffset = 0;
	spawner->cooldown = config.cooldown;
	spawner->bulletCount = config.bulletCount;
	spawner->bulletSpeed = config.bulletSpeed;
	spawner->spreadAngle = config.spreadAngle;
	spawner->rotationSpeed = config.rotationSpeed;
	spawner->randomizeSpeed = config.randomizeSpeed;
	spawner->speedVariation = config.speedVariation;
	spawner->bulletColor = config.bulletColor;
	spawner->bulletSize = config.bulletSize;
}
void Spawner_Update(BulletSpawner *spawner, Bullet bullets[], int *bulletCount,
                    Collectible collectibles[], int *collectibleCount,
                    Vector2 playerPos, float dt)
{
	if (!spawner->active)
		return;
	// Stop spawning if too many bullets exist
	if (*bulletCount >= MAX_BULLETS - 20)
		return;
	spawner->timer += dt;
	spawner->angleOffset += dt * spawner->rotationSpeed;
	if (spawner->timer >= spawner->cooldown)
	{
		spawner->timer = 0;
		int oldBulletCount = *bulletCount;
		switch (spawner->pattern)
		{
		case SPAWNER_PATTERN_CIRCLE:
			Spawner_PatternCircle(spawner, bullets, bulletCount);
			break;
		case SPAWNER_PATTERN_SPIRAL:
			Spawner_PatternSpiral(spawner, bullets, bulletCount);
			break;
		case SPAWNER_PATTERN_WAVE:
			Spawner_PatternWave(spawner, bullets, bulletCount);
			break;
		case SPAWNER_PATTERN_BURST:
			Spawner_PatternBurst(spawner, bullets, bulletCount);
			break;
		case SPAWNER_PATTERN_TARGETING:
			Spawner_PatternTargeting(spawner, bullets, bulletCount, playerPos);
			break;
		}
		
		// Randomly spawn collectibles
		if (*collectibleCount < MAX_COLLECTIBLES - 5)
		{
			float spawnRoll = (float)rand() / RAND_MAX;
			if (spawnRoll < COLLECTIBLE_SPAWN_CHANCE)
			{
				float typeRoll = (float)rand() / RAND_MAX;
				CollectibleType type = (typeRoll < HEALTH_POINT_SPAWN_WEIGHT) 
				                       ? COLLECTIBLE_HEALTH_POINT 
				                       : COLLECTIBLE_SCORE;
				Vector2 center = Vector2Add(spawner->position, (Vector2){25, 25});
				Collectible_Spawn(collectibles, collectibleCount, center, type);
			}
		}
	}
}
void Spawner_Draw(const BulletSpawner *spawner)
{
	if (!spawner->active)
		return;
	Vector2 center = Vector2Add(spawner->position, (Vector2){25, 25});
	DrawCircleV(center, 20, spawner->bulletColor);
	DrawCircleV(center, 18, BLACK);
	DrawCircleV(center, 15, spawner->bulletColor);
	float progress = 1.0f - (spawner->timer / spawner->cooldown);
	if (progress < 1.0f)
	{
		DrawRing(center, 22, 25, 0, progress * 360, 36,
		         (Color){255, 255, 255, 200});
	}
	if (spawner->pattern == SPAWNER_PATTERN_SPIRAL)
	{
		float angle = spawner->angleOffset * DEG2RAD;
		Vector2 indicator = {center.x + cosf(angle) * 18,
		                     center.y + sinf(angle) * 18};
		DrawLineEx(center, indicator, 3, WHITE);
	}
}
void Spawner_PatternCircle(BulletSpawner *spawner, Bullet bullets[],
                           int *bulletCount)
{
	float angleStep = spawner->spreadAngle / spawner->bulletCount;
	Vector2 center = Vector2Add(spawner->position, (Vector2){25, 25});
	for (int i = 0; i < spawner->bulletCount && *bulletCount < MAX_BULLETS; i++)
	{
		float angle = (angleStep * i + spawner->angleOffset) * DEG2RAD;
		float speed = spawner->bulletSpeed;
		if (spawner->randomizeSpeed)
		{
			speed += (rand() % (int)(spawner->speedVariation * 2)) -
			         spawner->speedVariation;
		}
		Vector2 velocity = {cosf(angle) * speed, sinf(angle) * speed};
		bullets[*bulletCount] = (Bullet){.position = center,
		                                 .velocity = velocity,
		                                 .radius = spawner->bulletSize,
		                                 .active = true};
		(*bulletCount)++;
	}
}
void Spawner_PatternSpiral(BulletSpawner *spawner, Bullet bullets[],
                           int *bulletCount)
{
	float angleStep = 360.0f / spawner->bulletCount;
	Vector2 center = Vector2Add(spawner->position, (Vector2){25, 25});
	for (int i = 0; i < spawner->bulletCount && *bulletCount < MAX_BULLETS; i++)
	{
		float angle = (angleStep * i + spawner->angleOffset) * DEG2RAD;
		Vector2 velocity = {cosf(angle) * spawner->bulletSpeed,
		                    sinf(angle) * spawner->bulletSpeed};
		bullets[*bulletCount] = (Bullet){.position = center,
		                                 .velocity = velocity,
		                                 .radius = spawner->bulletSize,
		                                 .active = true};
		(*bulletCount)++;
	}
}
void Spawner_PatternWave(BulletSpawner *spawner, Bullet bullets[],
                         int *bulletCount)
{
	Vector2 center = Vector2Add(spawner->position, (Vector2){25, 25});
	float angleStep = spawner->spreadAngle / (spawner->bulletCount - 1);
	float startAngle = -spawner->spreadAngle / 2 + spawner->angleOffset;
	for (int i = 0; i < spawner->bulletCount && *bulletCount < MAX_BULLETS; i++)
	{
		float angle = (startAngle + angleStep * i) * DEG2RAD;
		float waveOffset =
		    sinf(spawner->angleOffset * DEG2RAD * 2 + i * 0.5f) * 20.0f;
		Vector2 velocity = {cosf(angle) * (spawner->bulletSpeed + waveOffset),
		                    sinf(angle) * (spawner->bulletSpeed + waveOffset)};
		bullets[*bulletCount] = (Bullet){.position = center,
		                                 .velocity = velocity,
		                                 .radius = spawner->bulletSize,
		                                 .active = true};
		(*bulletCount)++;
	}
}
void Spawner_PatternBurst(BulletSpawner *spawner, Bullet bullets[],
                          int *bulletCount)
{
	float angleStep = spawner->spreadAngle / spawner->bulletCount;
	Vector2 center = Vector2Add(spawner->position, (Vector2){25, 25});
	for (int i = 0; i < spawner->bulletCount && *bulletCount < MAX_BULLETS; i++)
	{
		float angle = (angleStep * i + spawner->angleOffset) * DEG2RAD;
		float speed = spawner->bulletSpeed;
		if (spawner->randomizeSpeed)
		{
			speed += (rand() % (int)(spawner->speedVariation * 2)) -
			         spawner->speedVariation;
		}
		Vector2 velocity = {cosf(angle) * speed, sinf(angle) * speed};
		bullets[*bulletCount] = (Bullet){.position = center,
		                                 .velocity = velocity,
		                                 .radius = spawner->bulletSize,
		                                 .active = true};
		(*bulletCount)++;
	}
}
void Spawner_PatternTargeting(BulletSpawner *spawner, Bullet bullets[],
                              int *bulletCount, Vector2 playerPos)
{
	Vector2 center = Vector2Add(spawner->position, (Vector2){25, 25});
	Vector2 toPlayer = Vector2Subtract(playerPos, center);
	float baseAngle = atan2f(toPlayer.y, toPlayer.x);
	float halfSpread = (spawner->spreadAngle / 2) * DEG2RAD;
	float angleStep =
	    (spawner->spreadAngle * DEG2RAD) / (spawner->bulletCount - 1);
	for (int i = 0; i < spawner->bulletCount && *bulletCount < MAX_BULLETS; i++)
	{
		float angle = baseAngle - halfSpread + angleStep * i;
		Vector2 velocity = {cosf(angle) * spawner->bulletSpeed,
		                    sinf(angle) * spawner->bulletSpeed};
		bullets[*bulletCount] = (Bullet){.position = center,
		                                 .velocity = velocity,
		                                 .radius = spawner->bulletSize,
		                                 .active = true};
		(*bulletCount)++;
	}
}
void Bullet_Update(Bullet bullets[], int *bulletCount, float dt)
{
	for (int i = 0; i < *bulletCount; i++)
	{
		if (!bullets[i].active)
			continue;
		bullets[i].position.x += bullets[i].velocity.x * dt;
		bullets[i].position.y += bullets[i].velocity.y * dt;
		// Tighter bounds checking for better performance
		if (bullets[i].position.x < -100 || bullets[i].position.x > 5000 ||
		    bullets[i].position.y < -100 || bullets[i].position.y > 5000)
		{
			bullets[i].active = false;
		}
	}
	// Remove inactive bullets to prevent array growth
	int writeIndex = 0;
	for (int readIndex = 0; readIndex < *bulletCount; readIndex++)
	{
		if (bullets[readIndex].active)
		{
			if (writeIndex != readIndex)
			{
				bullets[writeIndex] = bullets[readIndex];
			}
			writeIndex++;
		}
	}
	*bulletCount = writeIndex;
}
void Bullet_Draw(const Bullet bullets[], int bulletCount)
{
	// Early exit if no bullets
	if (bulletCount == 0)
		return;
	for (int i = 0; i < bulletCount; i++)
	{
		if (!bullets[i].active)
			continue;
		DrawCircleV(bullets[i].position, bullets[i].radius + 2,
		            (Color){255, 255, 255, 50});
		DrawCircleV(bullets[i].position, bullets[i].radius, RED);
		DrawCircleV(bullets[i].position, bullets[i].radius - 1,
		            (Color){255, 100, 100, 255});
	}
}
bool Bullet_CheckCollision(const Bullet *bullet, Rectangle playerBounds)
{
	if (!bullet->active)
		return false;
	float closestX =
	    fmaxf(playerBounds.x,
	          fminf(bullet->position.x, playerBounds.x + playerBounds.width));
	float closestY =
	    fmaxf(playerBounds.y,
	          fminf(bullet->position.y, playerBounds.y + playerBounds.height));
	float dx = bullet->position.x - closestX;
	float dy = bullet->position.y - closestY;
	float distance = sqrtf(dx * dx + dy * dy);
	return distance < bullet->radius;
}

// Collectible system implementation
void Collectible_Spawn(Collectible collectibles[], int *collectibleCount,
                       Vector2 position, CollectibleType type)
{
	if (*collectibleCount >= MAX_COLLECTIBLES)
		return;
	
	// Random direction
	float angle = ((float)rand() / RAND_MAX) * 360.0f * DEG2RAD;
	Vector2 velocity = {
		cosf(angle) * COLLECTIBLE_SPEED,
		sinf(angle) * COLLECTIBLE_SPEED
	};
	
	float size = (type == COLLECTIBLE_HEALTH_POINT) 
	             ? HEALTH_POINT_SIZE 
	             : SCORE_ITEM_SIZE;
	
	collectibles[*collectibleCount] = (Collectible){
		.position = position,
		.velocity = velocity,
		.radius = size,
		.type = type,
		.active = true,
		.lifetime = COLLECTIBLE_LIFETIME
	};
	(*collectibleCount)++;
}

void Collectible_Update(Collectible collectibles[], int *collectibleCount, float dt)
{
	for (int i = 0; i < *collectibleCount; i++)
	{
		if (!collectibles[i].active)
			continue;
		
		// Update position
		collectibles[i].position.x += collectibles[i].velocity.x * dt;
		collectibles[i].position.y += collectibles[i].velocity.y * dt;
		
		// Update lifetime
		collectibles[i].lifetime -= dt;
		if (collectibles[i].lifetime <= 0)
		{
			collectibles[i].active = false;
		}
		
		// Bounds checking
		if (collectibles[i].position.x < -100 || collectibles[i].position.x > 5000 ||
		    collectibles[i].position.y < -100 || collectibles[i].position.y > 5000)
		{
			collectibles[i].active = false;
		}
	}
	
	// Remove inactive collectibles
	int writeIndex = 0;
	for (int readIndex = 0; readIndex < *collectibleCount; readIndex++)
	{
		if (collectibles[readIndex].active)
		{
			if (writeIndex != readIndex)
			{
				collectibles[writeIndex] = collectibles[readIndex];
			}
			writeIndex++;
		}
	}
	*collectibleCount = writeIndex;
}

void Collectible_Draw(const Collectible collectibles[], int collectibleCount)
{
	for (int i = 0; i < collectibleCount; i++)
	{
		if (!collectibles[i].active)
			continue;
		
		Color color;
		if (collectibles[i].type == COLLECTIBLE_HEALTH_POINT)
		{
			color = HEALTH_POINT_COLOR;
			// Draw cross for health
			DrawCircleV(collectibles[i].position, collectibles[i].radius + 2, 
			           (Color){255, 255, 255, 100});
			DrawCircleV(collectibles[i].position, collectibles[i].radius, color);
			
			float size = collectibles[i].radius * 0.6f;
			DrawRectangle(collectibles[i].position.x - size / 4, 
			             collectibles[i].position.y - size,
			             size / 2, size * 2, WHITE);
			DrawRectangle(collectibles[i].position.x - size, 
			             collectibles[i].position.y - size / 4,
			             size * 2, size / 2, WHITE);
		}
		else // COLLECTIBLE_SCORE
		{
			color = SCORE_ITEM_COLOR;
			// Draw star for score
			DrawCircleV(collectibles[i].position, collectibles[i].radius + 2, 
			           (Color){255, 255, 255, 100});
			DrawCircleV(collectibles[i].position, collectibles[i].radius, color);
			DrawCircleV(collectibles[i].position, collectibles[i].radius - 2, 
			           (Color){255, 235, 100, 255});
		}
	}
}

bool Collectible_CheckCollection(const Collectible *collectible, Rectangle playerBounds)
{
	if (!collectible->active)
		return false;
	
	float closestX = fmaxf(playerBounds.x,
	                       fminf(collectible->position.x, 
	                             playerBounds.x + playerBounds.width));
	float closestY = fmaxf(playerBounds.y,
	                       fminf(collectible->position.y, 
	                             playerBounds.y + playerBounds.height));
	float dx = collectible->position.x - closestX;
	float dy = collectible->position.y - closestY;
	float distance = sqrtf(dx * dx + dy * dy);
	return distance < collectible->radius;
}

