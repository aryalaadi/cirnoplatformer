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

#include "world.h"
#include "config.h"
#include "physics.h"
#include <math.h>
#include <stdio.h>
void World_Load(World *world, int levelIndex)
{
	Assets_Load(&world->assets);
	Level_Load(&world->level, levelIndex);
	Player_Init(&world->player, world->level.playerSpawn);
	world->camera.target = world->player.position;
	world->camera.offset = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
	world->camera.rotation = 0.0f;
	world->camera.zoom = 1.0f;
	world->spawnerCount = 0;
	world->bulletCount = 0;
	world->collectibleCount = 0;
	world->parryEffectCount = 0;

	for (int y = 0; y < world->level.height; y++)
	{
		for (int x = 0; x < world->level.width; x++)
		{
			int tile = Level_GetTile(&world->level, x, y);
			SpawnerPattern pattern = SPAWNER_PATTERN_CIRCLE; // Default value
			bool isSpawnerTile = false;
			
			if (tile == TILE_SPAWNER_CIRCLE)
			{
				pattern = SPAWNER_PATTERN_CIRCLE;
				isSpawnerTile = true;
			}
			else if (tile == TILE_SPAWNER_SPIRAL)
			{
				pattern = SPAWNER_PATTERN_SPIRAL;
				isSpawnerTile = true;
			}
			else if (tile == TILE_SPAWNER_WAVE)
			{
				pattern = SPAWNER_PATTERN_WAVE;
				isSpawnerTile = true;
			}
			else if (tile == TILE_SPAWNER_BURST)
			{
				pattern = SPAWNER_PATTERN_BURST;
				isSpawnerTile = true;
			}
			
			if (isSpawnerTile && world->spawnerCount < MAX_SPAWNERS)
			{
				Vector2 pos = {x * TILE_SIZE, y * TILE_SIZE};
				Spawner_Init(&world->spawners[world->spawnerCount], pos,
				             pattern);
				world->spawnerCount++;
			}
		}
	}
}
void World_Unload(World *world)
{
	Level_Unload(&world->level);
	Assets_Unload(&world->assets);
}
void World_Update(World *world, float dt, const KeyBindings *keys)
{
	Player_Update(&world->player, dt, &world->assets, keys);
	Physics_ApplyGravity(&world->player, dt);
	Physics_MoveX(&world->player, &world->level, dt);
	Physics_MoveY(&world->player, &world->level, dt);
	
	// Check if player has fallen out of bounds
	if (World_IsPlayerOutOfBounds(world))
	{
		world->player.health = 0;  // Instant death
		return;
	}
	// Only update spawners within range of player for performance
	Vector2 playerPos = world->player.position;





	const float MAX_SPAWNER_DIST_SQ = 2250000.0f; // 1500^2 (~30 tiles)
	for (int i = 0; i < world->spawnerCount; i++)
	{
		float dx = world->spawners[i].position.x - playerPos.x;
		float dy = world->spawners[i].position.y - playerPos.y;
		float distSq = dx * dx + dy * dy;
		// Only update spawners within range
		if (distSq < MAX_SPAWNER_DIST_SQ)
		{
			Spawner_Update(&world->spawners[i], world->bullets, &world->bulletCount,
			               world->collectibles, &world->collectibleCount,
			               world->player.position, dt);
		}
	}
	Bullet_Update(world->bullets, &world->bulletCount, dt);
	Collectible_Update(world->collectibles, &world->collectibleCount, dt);
	
	Rectangle playerBounds = Player_GetBounds(&world->player);
	
	// Get current input state for parry detection
	bool movingLeft = IsKeyDown(keys->moveLeft) || IsKeyDown(KEY_LEFT);
	bool movingRight = IsKeyDown(keys->moveRight) || IsKeyDown(KEY_RIGHT);
	
	// Only check bullet collisions if bullets exist
	if (world->bulletCount > 0)
	{
		for (int i = 0; i < world->bulletCount; i++)
		{
			// Skip parried bullets - they only hit spawners now
			if (world->bullets[i].isParried)
				continue;
			
			if (Bullet_CheckCollision(&world->bullets[i], playerBounds))
			{
				// Check if player can parry this bullet
				if (Player_CanParryBullet(&world->player, world->bullets[i].velocity, 
				                         movingLeft, movingRight))
				{
					// Parry successful! Reflect bullet back in opposite direction
					// Simply reverse the velocity and multiply by speed multiplier
					world->bullets[i].velocity.x *= -PARRIED_BULLET_SPEED_MULTIPLIER;
					world->bullets[i].velocity.y *= -PARRIED_BULLET_SPEED_MULTIPLIER;
					world->bullets[i].isParried = true;
				}
				else
				{
					// Normal hit - take damage
					Player_TakeDamage(&world->player, 1);
					world->bullets[i].active = false;
				}
			}
		}
	}
	
	// Check parried bullet collisions with spawners
	for (int i = 0; i < world->bulletCount; i++)
	{
		if (!world->bullets[i].isParried || !world->bullets[i].active)
			continue;
		
		// Check collision with all spawners
		for (int j = 0; j < world->spawnerCount; j++)
		{
			if (!world->spawners[j].active)
				continue;
			
			// Create rectangle for spawner hitbox (50x50 tile)
			Rectangle spawnerBounds = {
				world->spawners[j].position.x,
				world->spawners[j].position.y,
				50, 50
			};
			
			// Check if bullet hits spawner
			float closestX = fmaxf(spawnerBounds.x,
			                      fminf(world->bullets[i].position.x, 
			                            spawnerBounds.x + spawnerBounds.width));
			float closestY = fmaxf(spawnerBounds.y,
			                      fminf(world->bullets[i].position.y, 
			                            spawnerBounds.y + spawnerBounds.height));
			float dx = world->bullets[i].position.x - closestX;
			float dy = world->bullets[i].position.y - closestY;
			float distance = sqrtf(dx * dx + dy * dy);
			
			if (distance < world->bullets[i].radius)
			{
				// Hit! Damage spawner and destroy bullet
				Spawner_TakeDamage(&world->spawners[j], 1);
				world->bullets[i].active = false;
				break; // Bullet can only hit one spawner
			}
		}
	}
	
	int tileX = (int)((playerBounds.x + playerBounds.width / 2) / TILE_SIZE);
	int tileY = (int)((playerBounds.y + playerBounds.height) / TILE_SIZE);
	int tile = Level_GetTile(&world->level, tileX, tileY);
	world->player.lastTileStanding = tile;
	if (tile > 0)
	{
		TileEffect effect = Level_GetTileEffect(tile);
		if (effect.isCheckpoint && world->player.onGround)
		{
			world->player.lastCheckpoint =
			    (Vector2){tileX * TILE_SIZE, tileY * TILE_SIZE};
		}
		if (tile == TILE_SPIKE && world->player.onGround)
		{
			Player_TakeDamage(&world->player, 1);
			world->player.position = world->player.lastCheckpoint;
			world->player.velocity = (Vector2){0, 0};
		}
		else if (tile == TILE_DAMAGE && world->player.onGround)
		{
			if (world->player.damageTimer <= 0)
			{
				Player_TakeDamage(&world->player, 1);
				world->player.damageTimer = 0.5f;
			}
		}
	}
	if (tile == TILE_GRASS || tile == TILE_DIRT || tile == TILE_STONE ||
	    tile == TILE_CHECKPOINT)
	{
		// On safe tile
	}
	if (world->player.position.y > world->level.height * TILE_SIZE)
	{
		world->player.position = world->player.lastCheckpoint;
		world->player.velocity = (Vector2){0, 0};
		world->player.health = PLAYER_MAX_HEALTH;
		world->player.invulnerabilityTimer = 0;
	}
	
	// Calculate desired camera position (player center)
	Vector2 desiredCamera = {
		world->player.position.x + PLAYER_SIZE / 2,
		world->player.position.y + PLAYER_SIZE / 2
	};
	
	// Camera bounds - clamp desired position first
	float minCameraX = SCREEN_WIDTH / 2.0f;
	float maxCameraX = world->level.width * TILE_SIZE - SCREEN_WIDTH / 2.0f;
	float minCameraY = SCREEN_HEIGHT / 2.0f;
	float maxCameraY = world->level.height * TILE_SIZE - SCREEN_HEIGHT / 2.0f;
	
	// Clamp desired camera position to level bounds
	if (world->level.width * TILE_SIZE > SCREEN_WIDTH)
	{
		if (desiredCamera.x < minCameraX)
			desiredCamera.x = minCameraX;
		if (desiredCamera.x > maxCameraX)
			desiredCamera.x = maxCameraX;
	}
	else
	{
		desiredCamera.x = (world->level.width * TILE_SIZE) / 2.0f;
	}
	
	if (world->level.height * TILE_SIZE > SCREEN_HEIGHT)
	{
		if (desiredCamera.y < minCameraY)
			desiredCamera.y = minCameraY;
		if (desiredCamera.y > maxCameraY)
			desiredCamera.y = maxCameraY;
	}
	else
	{
		desiredCamera.y = (world->level.height * TILE_SIZE) / 2.0f;
	}
	
	// Apply smooth camera movement toward clamped desired position
	float smoothing = 0.15f;
	world->camera.target.x = desiredCamera.x * (1.0f - smoothing) +
	                         world->camera.target.x * smoothing;
	world->camera.target.y = desiredCamera.y * (1.0f - smoothing) +
	                         world->camera.target.y * smoothing;
}

bool World_IsPlayerOutOfBounds(const World *world)
{
	Rectangle playerBounds = Player_GetBounds(&world->player);
	float levelWidth = world->level.width * TILE_SIZE;
	float levelHeight = world->level.height * TILE_SIZE;
	
	// Check bottom boundary (most common - player falls off)
	if (playerBounds.y > levelHeight + OUT_OF_BOUNDS_MARGIN)
		return true;
	
	// Check top boundary
	if (playerBounds.y + playerBounds.height < -OUT_OF_BOUNDS_TOP_MARGIN)
		return true;
	
	// Check left boundary
	if (playerBounds.x + playerBounds.width < -OUT_OF_BOUNDS_SIDE_MARGIN)
		return true;
	
	// Check right boundary
	if (playerBounds.x > levelWidth + OUT_OF_BOUNDS_SIDE_MARGIN)
		return true;
	
	return false;
}

void World_Draw(const World *world)
{
	BeginMode2D(world->camera);
	Level_Draw(&world->level, &world->assets, world->camera);
	for (int i = 0; i < world->spawnerCount; i++)
	{
		Spawner_Draw(&world->spawners[i]);
	}
	Bullet_Draw(world->bullets, world->bulletCount);
	Collectible_Draw(world->collectibles, world->collectibleCount);
	Player_Draw(&world->player);
	EndMode2D();
}
bool World_LevelCompleted(const World *world)
{
	if (!world->level.hasGoal)
		return false;
	Rectangle playerBounds = Player_GetBounds(&world->player);
	Rectangle goalBounds = {world->level.goalPos.x, world->level.goalPos.y,
	                        TILE_SIZE, TILE_SIZE};
	return CheckCollisionRecs(playerBounds, goalBounds);
}

void World_ResetBullets(World *world)
{
	world->bulletCount = 0;
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		world->bullets[i].active = false;
	}
	
	// Also reset parry effects
	world->parryEffectCount = 0;
	for (int i = 0; i < MAX_PARRY_EFFECTS; i++)
	{
		world->parryEffects[i].active = false;
	}

	// reset the timer so player doesnt die on respawn
	for (int i = 0; i < world->spawnerCount; i++)
	{
		world->spawners[i].timer = 0.0f;
		world->spawners[i].health = SPAWNER_INITIAL_HEALTH;
		world->spawners[i].active = true;
	}
}

int World_CollectItems(World *world, int *healthPointsCollected, int *scoreCollected)
{
	Rectangle playerBounds = Player_GetBounds(&world->player);
	int totalCollected = 0;
	*healthPointsCollected = 0;
	*scoreCollected = 0;
	
	for (int i = 0; i < world->collectibleCount; i++)
	{
		if (!world->collectibles[i].active)
			continue;
			
		if (Collectible_CheckCollection(&world->collectibles[i], playerBounds))
		{
			world->collectibles[i].active = false;
			totalCollected++;
			
			if (world->collectibles[i].type == COLLECTIBLE_HEALTH_POINT)
			{
				*healthPointsCollected += HEALTH_POINT_VALUE;
			}
			else if (world->collectibles[i].type == COLLECTIBLE_SCORE)
			{
				*scoreCollected += SCORE_ITEM_VALUE;
			}
		}
	}
	
	return totalCollected;
}
