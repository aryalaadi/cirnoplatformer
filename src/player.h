#ifndef PLAYER_H
#define PLAYER_H
#include "assets.h"
#include "config.h"
#include "raylib.h"
typedef enum
{
	ANIM_IDLE,
	ANIM_RUN,
	ANIM_JUMP,
	ANIM_FALL,
	ANIM_WALL_SLIDE,
	ANIM_WALL_CLING,
	ANIM_FLOAT,
	ANIM_DAMAGE,
	ANIM_DEATH,
	ANIM_DUCK
} PlayerAnimState;
typedef struct
{
	Vector2 position;
	Vector2 velocity;
	bool onGround;
	int health;
	int maxHealth;
	float dashCooldown;
	float dashTimer;
	bool isFloating;
	float floatTimer;
	float floatCooldown;
	bool canDash;
	float invulnerabilityTimer;
	Vector2 lastCheckpoint;
	float coyoteTime;
	float jumpBufferTime;
	bool onWall;
	int wallDirection;
	float wallJumpTime;
	bool isClinging;
	float clingTimer;
	float damageTimer;
	int lastTileStanding;
	PlayerAnimState animState;
	int animFrame;
	float animTimer;
	bool facingRight;
	Texture2D spriteSheet;
	bool hasSprite;
	bool isSlowingDown;
	bool isDucking;
} Player;
void Player_Init(Player *p, Vector2 spawn);
void Player_Update(Player *p, float dt, Assets *assets,
                   const KeyBindings *keys);
void Player_Draw(const Player *p);
void Player_TakeDamage(Player *p, int amount);
void Player_Heal(Player *p, int amount);
bool Player_IsAlive(const Player *p);
Rectangle Player_GetBounds(const Player *p);
#endif
