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

#include "player.h"
#include "config.h"
#include <math.h>
void Player_Init(Player *p, Vector2 spawn)
{
	p->position = spawn;
	p->velocity = (Vector2){0};
	p->onGround = false;
	p->health = PLAYER_MAX_HEALTH;
	p->maxHealth = PLAYER_MAX_HEALTH;
	p->dashCooldown = 0;
	p->dashTimer = 0;
	p->canDash = true;
	p->isFloating = false;
	p->floatTimer = 0;
	p->floatCooldown = 0;
	p->invulnerabilityTimer = 0;
	p->lastCheckpoint = spawn;
	p->coyoteTime = 0;
	p->jumpBufferTime = 0;
	p->onWall = false;
	p->wallDirection = 0;
	p->wallJumpTime = 0;
	p->isClinging = false;
	p->clingTimer = 0;
	p->damageTimer = 0;
	p->lastTileStanding = -1;
	p->animState = ANIM_IDLE;
	p->animFrame = 0;
	p->animTimer = 0;
	p->facingRight = true;
	p->hasSprite = false;
	p->isSlowingDown = false;
	p->isDucking = false;
	p->parryWindowTimer = 0;
	p->parryCooldown = 0;
	p->isParryActive = false;
	if (FileExists("assets/sprites/player.png"))
	{
		p->spriteSheet = LoadTexture("assets/sprites/player.png");
		p->hasSprite = true;
	}
}
void Player_Update(Player *p, float dt, Assets *assets, const KeyBindings *keys)
{
	if (p->dashCooldown > 0)
		p->dashCooldown -= dt;
	if (p->dashTimer > 0)
		p->dashTimer -= dt;
	if (p->floatCooldown > 0)
		p->floatCooldown -= dt;
	if (p->floatTimer > 0)
		p->floatTimer -= dt;
	if (p->invulnerabilityTimer > 0)
		p->invulnerabilityTimer -= dt;
	if (p->coyoteTime > 0)
		p->coyoteTime -= dt;
	if (p->jumpBufferTime > 0)
		p->jumpBufferTime -= dt;
	if (p->wallJumpTime > 0)
		p->wallJumpTime -= dt;
	if (p->damageTimer > 0)
		p->damageTimer -= dt;
	if (p->clingTimer > 0)
		p->clingTimer -= dt;
	if (p->parryWindowTimer > 0)
		p->parryWindowTimer -= dt;
	if (p->parryCooldown > 0)
		p->parryCooldown -= dt;
	
	// Update parry active state
	p->isParryActive = (p->parryWindowTimer > 0);
	
	if (p->isFloating && p->floatTimer <= 0)
	{
		p->isFloating = false;
	}
	if (p->onGround)
	{
		p->coyoteTime = PLAYER_COYOTE_TIME;
	}
	if (p->onWall && !p->onGround)
	{
		p->wallJumpTime = PLAYER_WALL_JUMP_TIME;
	}
	if (IsKeyPressed(keys->floatKey) && !p->onGround && !p->isFloating &&
	    p->floatCooldown <= 0)
	{
		p->isFloating = true;
		p->floatTimer = PLAYER_FLOAT_DURATION;
		p->floatCooldown = PLAYER_FLOAT_COOLDOWN;
		p->velocity.y = 0;
	}
	bool wantsToCling = IsKeyDown(keys->wallCling);
	if (p->onWall && !p->onGround && wantsToCling)
	{
		if (!p->isClinging)
		{
			p->isClinging = true;
			p->clingTimer = PLAYER_WALL_CLING_DURATION;
		}
		if (p->isFloating)
		{
			p->isFloating = false;
			p->floatTimer = 0;
		}
	}
	else
	{
		p->isClinging = false;
		p->clingTimer = 0;
	}
	bool movingLeft = IsKeyDown(keys->moveLeft) || IsKeyDown(KEY_LEFT);
	bool movingRight = IsKeyDown(keys->moveRight) || IsKeyDown(KEY_RIGHT);
	bool movingUp =
	    IsKeyDown(keys->jump) || IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
	bool movingDown = IsKeyDown(KEY_DOWN);
	
	// Duck mode: sticky behavior - once ducking, stay ducking until key released
	if (IsKeyDown(KEY_DOWN) && p->onGround)
	{
		p->isDucking = true;
	}
	else if (!IsKeyDown(KEY_DOWN))
	{
		p->isDucking = false;
	}
	// else: maintain current isDucking state if key held but not on ground
	
	if (IsKeyPressed(keys->dash) && p->dashCooldown <= 0 && p->canDash &&
	    !p->isFloating)
	{
		p->dashCooldown = PLAYER_DASH_COOLDOWN;
		p->dashTimer = PLAYER_DASH_DURATION;
		float dashSpeed = PLAYER_SPEED * PLAYER_DASH_SPEED_MULTIPLIER;
		Vector2 dashDir = {0, 0};
		if (movingLeft)
			dashDir.x = -1;
		if (movingRight)
			dashDir.x = 1;
		if (movingUp && !p->isDucking)
			dashDir.y = -1;
		if (movingDown && !p->isDucking)
			dashDir.y = 1;
		if (dashDir.x == 0 && dashDir.y == 0)
		{
			dashDir.x = p->facingRight ? 1 : -1;
		}
		float magnitude = sqrtf(dashDir.x * dashDir.x + dashDir.y * dashDir.y);
		if (magnitude > 0)
		{
			dashDir.x /= magnitude;
			dashDir.y /= magnitude;
		}
		p->velocity.x = dashDir.x * dashSpeed;
		p->velocity.y = dashDir.y * dashSpeed;
		if (p->isClinging)
		{
			p->isClinging = false;
			p->clingTimer = 0;
		}
	}
	if (p->dashTimer <= 0 && !p->isFloating)
	{
		p->velocity.x = 0;
		// Duck mode: only horizontal movement allowed
		if (movingLeft)
		{
			p->velocity.x -= PLAYER_SPEED;
			p->facingRight = false;
		}
		if (movingRight)
		{
			p->velocity.x += PLAYER_SPEED;
			p->facingRight = true;
		}
	}
	else if (p->isFloating)
	{
		p->velocity.x = 0;
		if (movingLeft)
		{
			p->velocity.x -= PLAYER_SPEED * PLAYER_FLOAT_SPEED_MULTIPLIER;
			p->facingRight = false;
		}
		if (movingRight)
		{
			p->velocity.x += PLAYER_SPEED * PLAYER_FLOAT_SPEED_MULTIPLIER;
			p->facingRight = true;
		}
	}
	if (IsKeyDown(keys->slowDown))
	{
		p->velocity.x *= PLAYER_SLOWDOWN_MULTIPLIER;
		p->isSlowingDown = true;
		
		// Parry mechanic: slowdown + directional input
		// Activate parry window if cooldown is ready and player is holding slowdown + direction
		if (p->parryCooldown <= 0 && (movingLeft || movingRight))
		{
			// Only activate parry if it's not already active
			if (p->parryWindowTimer <= 0)
			{
				p->parryWindowTimer = PARRY_WINDOW_TIME;
				p->parryCooldown = PARRY_COOLDOWN;
			}
		}
	}
	else
	{
		p->isSlowingDown = false;
	}
	bool jumpPressed = IsKeyPressed(
	    keys->jump); //|| IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP);
	if (jumpPressed)
	{
		p->jumpBufferTime = PLAYER_JUMP_BUFFER_TIME;
		// Exit duck mode when jumping
		if (p->isDucking)
		{
			p->isDucking = false;
		}
		if (p->isFloating)
		{
			p->isFloating = false;
			p->floatTimer = 0;
		}
	}
	bool canJump = (p->onGround || p->coyoteTime > 0);
	if (canJump && p->jumpBufferTime > 0)
	{
		float jumpSpeed = PLAYER_JUMP_SPEED;
		if (p->lastTileStanding == TILE_JUMP_BOOST)
		{
			jumpSpeed *= TILE_JUMP_BOOST_MULTIPLIER;
		}
		p->velocity.y = -jumpSpeed;
		p->onGround = false;
		p->coyoteTime = 0;
		p->jumpBufferTime = 0;
		if (assets)
		{
			Assets_PlayJumpSound(assets);
		}
	}
	else if ((p->wallJumpTime > 0 || p->isClinging) && p->jumpBufferTime > 0 &&
	         !p->onGround)
	{
		p->velocity.y = -PLAYER_JUMP_SPEED;
		p->velocity.x = p->wallDirection * PLAYER_SPEED * PLAYER_WALL_JUMP_SPEED_MULTIPLIER;
		p->wallJumpTime = 0;
		p->jumpBufferTime = 0;
		p->onWall = false;
		p->isClinging = false;
		p->clingTimer = 0;
		if (assets)
		{
			Assets_PlayJumpSound(assets);
		}
	}
	bool jumpHeld =
	    IsKeyDown(keys->jump) || IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
	if (!jumpHeld && p->velocity.y < -100 && p->dashTimer <= 0)
	{
		p->velocity.y = -100;
	}
	PlayerAnimState newState = ANIM_IDLE;
	if (!Player_IsAlive(p))
	{
		newState = ANIM_DEATH;
	}
	else if (p->isDucking)
	{
		newState = ANIM_DUCK;
	}
	else if (p->isFloating)
	{
		newState = ANIM_FLOAT;
	}
	else if (p->dashTimer > 0)
	{
		newState = ANIM_RUN;
	}
	else if (p->invulnerabilityTimer > 0 &&
	         ((int)(p->invulnerabilityTimer * 10) % 2 == 0))
	{
		newState = ANIM_DAMAGE;
	}
	else if (p->isClinging)
	{
		newState = ANIM_WALL_CLING;
	}
	else if (p->onWall && !p->onGround && p->velocity.y > 0)
	{
		newState = ANIM_WALL_SLIDE;
	}
	else if (!p->onGround)
	{
		if (p->velocity.y < 0)
		{
			newState = ANIM_JUMP;
		}
		else
		{
			newState = ANIM_FALL;
		}
	}
	else if (p->velocity.x != 0)
	{
		newState = ANIM_RUN;
	}
	else
	{
		newState = ANIM_IDLE;
	}
	if (newState != p->animState)
	{
		p->animState = newState;
		p->animFrame = 0;
		p->animTimer = 0;
	}
	float frameRate = 10.0f;
	switch (p->animState)
	{
	case ANIM_IDLE:
		frameRate = 6.0f;
		break;
	case ANIM_RUN:
		frameRate = p->dashTimer > 0 ? 20.0f : 12.0f;
		break;
	case ANIM_JUMP:
		frameRate = 8.0f;
		break;
	case ANIM_FALL:
		frameRate = 6.0f;
		break;
	case ANIM_WALL_SLIDE:
		frameRate = 8.0f;
		break;
	case ANIM_WALL_CLING:
		frameRate = 4.0f;
		break;
	case ANIM_FLOAT:
		frameRate = 8.0f;
		break;
	case ANIM_DAMAGE:
		frameRate = 10.0f;
		break;
	case ANIM_DEATH:
		frameRate = 6.0f;
		break;
	case ANIM_DUCK:
		frameRate = 4.0f;
		break;
	}
	p->animTimer += dt;
	if (p->animTimer >= 1.0f / frameRate)
	{
		p->animTimer = 0;
		p->animFrame++;
		int maxFrames = 4;
		switch (p->animState)
		{
		case ANIM_IDLE:
			maxFrames = 4;
			break;
		case ANIM_RUN:
			maxFrames = 6;
			break;
		case ANIM_JUMP:
			maxFrames = 3;
			break;
		case ANIM_FALL:
			maxFrames = 2;
			break;
		case ANIM_WALL_SLIDE:
			maxFrames = 3;
			break;
		case ANIM_WALL_CLING:
			maxFrames = 2;
			break;
		case ANIM_FLOAT:
			maxFrames = 4;
			break;
		case ANIM_DAMAGE:
			maxFrames = 2;
			break;
		case ANIM_DEATH:
			maxFrames = 4;
			break;
		case ANIM_DUCK:
			maxFrames = 2;
			break;
		}
		if (p->animFrame >= maxFrames)
		{
			if (p->animState == ANIM_DEATH)
			{
				p->animFrame = maxFrames - 1;
			}
			else
			{
				p->animFrame = 0;
			}
		}
	}
}
void Player_TakeDamage(Player *p, int amount)
{
	if (p->invulnerabilityTimer <= 0)
	{
		p->health -= amount;
		if (p->health < 0)
			p->health = 0;
		p->invulnerabilityTimer = 1.0f;
	}
}
void Player_Heal(Player *p, int amount)
{
	p->health += amount;
	if (p->health > p->maxHealth)
		p->health = p->maxHealth;
}
bool Player_IsAlive(const Player *p) { return p->health > 0; }
void Player_Draw(const Player *p)
{
	if (p->hasSprite)
	{
		Rectangle sourceRect = {p->animFrame * PLAYER_SIZE,
		                        p->animState * PLAYER_SIZE, PLAYER_SIZE,
		                        PLAYER_SIZE};
		if (!p->facingRight)
		{
			sourceRect.width = -PLAYER_SIZE;
		}
		Rectangle destRect = {p->position.x, p->position.y, PLAYER_SIZE,
		                      PLAYER_SIZE};
		Vector2 origin = {0, 0};
		Color tint = WHITE;
		if (p->invulnerabilityTimer > 0 &&
		    ((int)(p->invulnerabilityTimer * 20) % 2 == 0))
		{
			tint = (Color){255, 100, 100, 255};
		}
		DrawTexturePro(p->spriteSheet, sourceRect, destRect, origin, 0.0f,
		               tint);
	}
	else
	{
		Color playerColor = RED;
		switch (p->animState)
		{
		case ANIM_IDLE:
			playerColor = RED;
			break;
		case ANIM_RUN:
			playerColor = (Color){255, 100, 100, 255};
			break;
		case ANIM_JUMP:
			playerColor = (Color){255, 150, 0, 255};
			break;
		case ANIM_FALL:
			playerColor = (Color){200, 0, 0, 255};
			break;
		case ANIM_WALL_SLIDE:
			playerColor = PURPLE;
			break;
		case ANIM_WALL_CLING:
			playerColor = BLUE;
			break;
		case ANIM_FLOAT:
			playerColor = SKYBLUE;
			break;
		case ANIM_DAMAGE:
			playerColor = WHITE;
			break;
		case ANIM_DEATH:
			playerColor = DARKGRAY;
			break;
		case ANIM_DUCK:
			playerColor = (Color){255, 150, 150, 255};
			break;
		}
		if (p->invulnerabilityTimer > 0 &&
		    ((int)(p->invulnerabilityTimer * 20) % 2 == 0))
		{
			playerColor = WHITE;
		}
		if (p->dashTimer > 0)
		{
			playerColor = YELLOW;
			DrawRectangle((int)p->position.x - 5, (int)p->position.y - 5,
			              PLAYER_SIZE + 10, PLAYER_SIZE + 10,
			              (Color){255, 255, 0, 100});
		}
		if (p->isFloating)
		{
			playerColor = SKYBLUE;
			DrawRectangle((int)p->position.x - 8, (int)p->position.y - 8,
			              PLAYER_SIZE + 16, PLAYER_SIZE + 16,
			              (Color){135, 206, 235, 80});
			DrawRectangle((int)p->position.x - 4, (int)p->position.y - 4,
			              PLAYER_SIZE + 8, PLAYER_SIZE + 8,
			              (Color){135, 206, 235, 120});
		}
		
		DrawRectangle((int)p->position.x, (int)p->position.y, PLAYER_SIZE,
		              PLAYER_SIZE, playerColor);
		int triSize = 5;
		if (p->facingRight)
		{
			DrawTriangle((Vector2){p->position.x + PLAYER_SIZE,
			                       p->position.y + PLAYER_SIZE / 2},
			             (Vector2){p->position.x + PLAYER_SIZE + triSize,
			                       p->position.y + PLAYER_SIZE / 2 - triSize},
			             (Vector2){p->position.x + PLAYER_SIZE + triSize,
			                       p->position.y + PLAYER_SIZE / 2 + triSize},
			             YELLOW);
		}
		else
		{
			DrawTriangle(
			    (Vector2){p->position.x, p->position.y + PLAYER_SIZE / 2},
			    (Vector2){p->position.x - triSize,
			              p->position.y + PLAYER_SIZE / 2 - triSize},
			    (Vector2){p->position.x - triSize,
			              p->position.y + PLAYER_SIZE / 2 + triSize},
			    YELLOW);
		}
	}
	if (p->isClinging)
	{
		int offset = p->wallDirection > 0 ? PLAYER_SIZE : -5;
		DrawRectangle((int)p->position.x + offset, (int)p->position.y + 10, 5,
		              20, WHITE);
	}
	DrawRectangle((int)p->position.x + 10, (int)p->position.y + 10, 8, 8,
	              WHITE);
	DrawRectangle((int)p->position.x + 22, (int)p->position.y + 10, 8, 8,
	              WHITE);
	DrawRectangle((int)p->position.x + 12, (int)p->position.y + 12, 4, 4,
	              BLACK);
	DrawRectangle((int)p->position.x + 24, (int)p->position.y + 12, 4, 4,
	              BLACK);
	int barWidth = PLAYER_SIZE;
	int barHeight = 5;
	DrawRectangle((int)p->position.x, (int)p->position.y - 10, barWidth,
	              barHeight, BLACK);
	DrawRectangle((int)p->position.x, (int)p->position.y - 10,
	              (int)(barWidth * ((float)p->health / p->maxHealth)),
	              barHeight, GREEN);
	if (p->dashCooldown > 0)
	{
		float cooldownPercent = 1.0f - (p->dashCooldown / 1.0f);
		DrawRectangle((int)p->position.x, (int)p->position.y - 4, barWidth, 3,
		              DARKGRAY);
		DrawRectangle((int)p->position.x, (int)p->position.y - 4,
		              (int)(barWidth * cooldownPercent), 3, YELLOW);
	}
	else
	{
		DrawRectangle((int)p->position.x, (int)p->position.y - 4, barWidth, 3,
		              GOLD);
	}
	if (p->isFloating)
	{
		float floatPercent = p->floatTimer / 2.5f;
		DrawRectangle((int)p->position.x, (int)p->position.y + PLAYER_SIZE + 2,
		              barWidth, 3, DARKBLUE);
		DrawRectangle((int)p->position.x, (int)p->position.y + PLAYER_SIZE + 2,
		              (int)(barWidth * floatPercent), 3, SKYBLUE);
	}
	else if (p->floatCooldown > 0)
	{
		float cooldownPercent = 1.0f - (p->floatCooldown / 4.0f);
		DrawRectangle((int)p->position.x, (int)p->position.y + PLAYER_SIZE + 2,
		              barWidth, 3, DARKGRAY);
		DrawRectangle((int)p->position.x, (int)p->position.y + PLAYER_SIZE + 2,
		              (int)(barWidth * cooldownPercent), 3, LIGHTGRAY);
	}
	else
	{
		DrawRectangle((int)p->position.x, (int)p->position.y + PLAYER_SIZE + 2,
		              barWidth, 3, SKYBLUE);
	}
}

void Player_DrawHitbox(const Player *p)
{
	// Draw hitbox visualization when ducking or slowing down
	if (p->isDucking || p->isSlowingDown)
	{
		Rectangle hitbox = Player_GetBounds(p);
		Color hitboxColor;
		
		// Prioritize duck visualization if both are active
		if (p->isDucking)
		{
			hitboxColor = (Color){255, 200, 0, 120};
		}
		else if (p->isSlowingDown)
		{
			hitboxColor = (Color){0, 255, 255, 120};
		}
		
		DrawRectangleRec(hitbox, hitboxColor);
		DrawRectangleLinesEx(hitbox, 2, (Color){hitboxColor.r, hitboxColor.g, hitboxColor.b, 255});
	}
}

Rectangle Player_GetBounds(const Player *p)
{
	// Base hitbox is 80% of sprite size (20px instead of 25px)
	float hitboxWidth = PLAYER_SIZE * 0.8f;
	float hitboxHeight = PLAYER_SIZE * 0.85f; // Slightly taller: 85% instead of 80%
	
	// When ducking, reduce height to 50%
	if (p->isDucking)
	{
		hitboxHeight = PLAYER_SIZE * 0.5f; // Slightly taller: 50% instead of 40%
	}
	
	// When slowing down, further reduce hitbox to 80% (total 64% of original)
	if (p->isSlowingDown)
	{
		hitboxWidth *= 0.8f;
		hitboxHeight *= 0.8f;
	}
	
	// Center the smaller hitbox within the sprite
	float offsetX = (PLAYER_SIZE - hitboxWidth) / 2.0f;
	float offsetY = PLAYER_SIZE - hitboxHeight; // Align to bottom
	
	return (Rectangle){p->position.x + offsetX, p->position.y + offsetY, hitboxWidth, hitboxHeight};
}

bool Player_CanParryBullet(const Player *p, Vector2 bulletVelocity, bool movingLeft, bool movingRight)
{
	// Player must be in active parry window
	if (!p->isParryActive || p->parryWindowTimer <= 0)
		return false;
	
	// Player must be holding slowdown (already checked if we're here, but safety check)
	if (!p->isSlowingDown)
		return false;
	
	// Calculate angle of bullet's velocity (direction it's moving)
	float bulletAngle = atan2f(bulletVelocity.y, bulletVelocity.x) * RAD2DEG;
	
	// Normalize angle to [0, 360)
	if (bulletAngle < 0)
		bulletAngle += 360.0f;
	
	// Determine which direction the bullet is traveling
	// Moving right: angles from -90 to +90 (or 270 to 90 in normalized form)
	// Moving left: angles from 90 to 270
	bool bulletMovingRight = (bulletAngle >= 270.0f || bulletAngle < 90.0f);
	bool bulletMovingLeft = (bulletAngle >= 90.0f && bulletAngle < 270.0f);
	
	// Parry succeeds if player is pressing opposite to bullet direction
	// Bullet moving right -> player pressing left
	// Bullet moving left -> player pressing right
	if (bulletMovingRight && movingLeft)
		return true;
	if (bulletMovingLeft && movingRight)
		return true;
	
	return false;
}
