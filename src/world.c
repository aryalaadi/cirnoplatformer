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
	printf("[DEBUG] Loading spawners from level (size: %dx%d)\n",
	       world->level.width, world->level.height);
	for (int y = 0; y < world->level.height; y++)
	{
		for (int x = 0; x < world->level.width; x++)
		{
			int tile = Level_GetTile(&world->level, x, y);
			SpawnerPattern pattern = (SpawnerPattern)-1;
			if (tile == TILE_SPAWNER_CIRCLE)
				pattern = SPAWNER_PATTERN_CIRCLE;
			else if (tile == TILE_SPAWNER_SPIRAL)
				pattern = SPAWNER_PATTERN_SPIRAL;
			else if (tile == TILE_SPAWNER_WAVE)
				pattern = SPAWNER_PATTERN_WAVE;
			else if (tile == TILE_SPAWNER_BURST)
				pattern = SPAWNER_PATTERN_BURST;
			if (pattern != -1 && world->spawnerCount < MAX_SPAWNERS)
			{
				Vector2 pos = {x * TILE_SIZE, y * TILE_SIZE};
				printf("[DEBUG] Creating spawner #%d: pattern=%d at (%d, %d) "
				       "tile=%d\n",
				       world->spawnerCount, pattern, x, y, tile);
				Spawner_Init(&world->spawners[world->spawnerCount], pos,
				             pattern);
				world->spawnerCount++;
			}
		}
	}
	printf("[DEBUG] Total spawners created: %d\n", world->spawnerCount);
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
	// Only update spawners within range of player for performance
	Vector2 playerPos = world->player.position;
	for (int i = 0; i < world->spawnerCount; i++)
	{
		float dx = world->spawners[i].position.x - playerPos.x;
		float dy = world->spawners[i].position.y - playerPos.y;
		float distSq = dx * dx + dy * dy;
		// Only update spawners within ~30 tiles (1500 pixels)
		if (distSq < 2250000.0f) // 1500^2
		{
			Spawner_Update(&world->spawners[i], world->bullets, &world->bulletCount,
			               world->player.position, dt);
		}
	}
	Bullet_Update(world->bullets, &world->bulletCount, dt);
	Rectangle playerBounds = Player_GetBounds(&world->player);
	// Only check bullet collisions if bullets exist
	if (world->bulletCount > 0)
	{
		for (int i = 0; i < world->bulletCount; i++)
		{
			if (Bullet_CheckCollision(&world->bullets[i], playerBounds))
			{
				Player_TakeDamage(&world->player, 1);
				world->bullets[i].active = false;
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
	bool onSafeTile = false;
	if (tile == TILE_GRASS || tile == TILE_DIRT || tile == TILE_STONE ||
	    tile == TILE_CHECKPOINT)
	{
		onSafeTile = true;
	}
	if (world->player.position.y > world->level.height * TILE_SIZE)
	{
		world->player.position = world->player.lastCheckpoint;
		world->player.velocity = (Vector2){0, 0};
		world->player.health = PLAYER_MAX_HEALTH;
		world->player.invulnerabilityTimer = 0;
	}
	float smoothing = 0.15f;
	world->camera.target.x = world->player.position.x +
	                         (PLAYER_SIZE / 2) * (1.0f - smoothing) +
	                         world->camera.target.x * smoothing;
	world->camera.target.y = world->player.position.y +
	                         (PLAYER_SIZE / 2) * (1.0f - smoothing) +
	                         world->camera.target.y * smoothing;
	if (world->camera.target.x < SCREEN_WIDTH / 2)
		world->camera.target.x = SCREEN_WIDTH / 2;
	if (world->camera.target.x >
	    world->level.width * TILE_SIZE - SCREEN_WIDTH / 2)
		world->camera.target.x =
		    world->level.width * TILE_SIZE - SCREEN_WIDTH / 2;
	if (world->camera.target.y < SCREEN_HEIGHT / 2)
		world->camera.target.y = SCREEN_HEIGHT / 2;
	if (world->camera.target.y >
	    world->level.height * TILE_SIZE - SCREEN_HEIGHT / 2)
		world->camera.target.y =
		    world->level.height * TILE_SIZE - SCREEN_HEIGHT / 2;
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

	// reset the timer so player doesnt die on respawn
	for (int i = 0; i < world->spawnerCount; i++)
	{
		world->spawners[i].timer = 0.0f;
	}
}
