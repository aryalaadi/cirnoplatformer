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

#include "physics.h"
#include "config.h"
void Physics_ApplyGravity(Player *p, float dt)
{
	if (!p)
		return;
	if (p->isFloating)
	{
		p->velocity.y += GRAVITY * 0.05f * dt;
		if (p->velocity.y > 50.0f)
			p->velocity.y = 50.0f;
	}
	else if (p->isClinging && p->clingTimer > 0)
	{
		p->velocity.y = 0;
	}
	else if (p->isClinging && p->clingTimer <= 0)
	{
		p->velocity.y += GRAVITY * 0.2f * dt;
		if (p->velocity.y > 50.0f)
			p->velocity.y = 50.0f;
	}
	else
	{
		p->velocity.y += GRAVITY * dt;
	}
}
void Physics_MoveX(Player *p, const Level *lvl, float dt)
{
	p->position.x += p->velocity.x * dt;
	Rectangle pBounds = Player_GetBounds(p);
	int left = (int)pBounds.x / TILE_SIZE;
	int right = (int)(pBounds.x + pBounds.width - 1) / TILE_SIZE;
	int top = (int)pBounds.y / TILE_SIZE;
	int bottom = (int)(pBounds.y + pBounds.height - 1) / TILE_SIZE;
	p->onWall = false;
	p->wallDirection = 0;
	for (int y = top; y <= bottom; y++)
	{
		if (p->velocity.x < 0 && Level_IsSolid(lvl, left, y))
		{
			p->position.x = (left + 1) * TILE_SIZE;
			p->velocity.x = 0;
			p->onWall = true;
			p->wallDirection = 1;
		}
		if (p->velocity.x > 0 && Level_IsSolid(lvl, right, y))
		{
			p->position.x = right * TILE_SIZE - PLAYER_SIZE;
			p->velocity.x = 0;
			p->onWall = true;
			p->wallDirection = -1;
		}
	}
}
void Physics_MoveY(Player *p, const Level *lvl, float dt)
{
	p->position.y += p->velocity.y * dt;
	Rectangle pBounds = Player_GetBounds(p);
	int left = (int)pBounds.x / TILE_SIZE;
	int right = (int)(pBounds.x + pBounds.width - 1) / TILE_SIZE;
	int top = (int)pBounds.y / TILE_SIZE;
	int bottom = (int)(pBounds.y + pBounds.height - 1) / TILE_SIZE;
	p->onGround = false;
	for (int x = left; x <= right; x++)
	{
		if (p->velocity.y < 0 && Level_IsSolid(lvl, x, top))
		{
			p->position.y = (top + 1) * TILE_SIZE;
			p->velocity.y = 0;
		}
		if (p->velocity.y >= 0 && Level_IsSolid(lvl, x, bottom))
		{
			p->position.y = bottom * TILE_SIZE - PLAYER_SIZE;
			p->velocity.y = 0;
			p->onGround = true;
		}
	}
}
