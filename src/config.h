#ifndef CONFIG_H
#define CONFIG_H
#include "raylib.h"
#include <stdbool.h>

// This is the default game res, too but will do for now.
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Might have to change this depending on how I want to balance the game.
#define TILE_SIZE 50
#define TARGET_FPS 60

// 128x128 should be more enough, Larger levels sound fun but not efficient
// give how my engine is rendering levels.
#define MIN_WORLD_WIDTH 20
#define MIN_WORLD_HEIGHT 15
#define MAX_WORLD_WIDTH 128
#define MAX_WORLD_HEIGHT 128

// Cirno is small, this is also the hitbox size.
#define PLAYER_SIZE 25

// Dont ask why.
#define MAX_LEVELS 100

// Probably Have to tweak this to balance.
#define PLAYER_SPEED 250.0f
#define PLAYER_JUMP_SPEED 350.0f
#define GRAVITY 1200.0f
#define PLAYER_MAX_HEALTH 3

// Random numbers for now
#define MAX_MUSIC_FILES 50
#define MAX_ENTITIES 100
#define MAX_SAVE_SLOTS 5

// TODO: Right now only 9 levels, more worlds and more level
#define BASE_LEVEL_COUNT 9

// TODO: Implement mulitple worlds and custom worlds
#define BACKGROUND_TILE_START 20
#define BASE_WORLD_COUNT 9
#define LEVELS_PER_WORLD 9
#define MAX_CUSTOM_WORLDS 100

// These are the indexes for the basic tiles, I will have to add more.
// TODO: tiles related to ice (slippery)
typedef enum
{
	TILE_EMPTY = 0,
	TILE_GRASS = 1,
	TILE_DIRT = 2,
	TILE_STONE = 3,
	TILE_GOAL = 4,
	TILE_DAMAGE = 5,
	TILE_JUMP_BOOST = 6,
	TILE_SPIKE = 7,
	TILE_CHECKPOINT = 8,
	TILE_SPAWNER_CIRCLE = 9,
	TILE_SPAWNER_SPIRAL = 10,
	TILE_SPAWNER_WAVE = 11,
	TILE_SPAWNER_BURST = 12
} TileType;

// TODO: Implement more patterns.
typedef enum
{
	SPAWNER_PATTERN_CIRCLE,
	SPAWNER_PATTERN_SPIRAL,
	SPAWNER_PATTERN_WAVE,
	SPAWNER_PATTERN_BURST,
	SPAWNER_PATTERN_TARGETING
} SpawnerPattern;

// This is used to define every bullet spawner and can be used to create many
// patterns.
// TODO: integrate this in the level editor for custom spawners.
typedef struct
{
	Vector2 position;
	SpawnerPattern pattern;
	float cooldown;
	float timer;
	bool active;
	int bulletCount;
	float bulletSpeed;
	float angleOffset;
	float spreadAngle;
	float rotationSpeed;
	bool randomizeSpeed;
	float speedVariation;
	Color bulletColor;
	float bulletSize;
} BulletSpawner;

typedef struct
{
	Vector2 position;
	Vector2 velocity;
	float radius;
	bool active;
} Bullet;

// TODO: add a player movement multiplier to implement slipery ice
typedef struct
{
	bool hasDamage;
	int damageAmount;
	bool hasJumpBoost;
	float jumpBoostMultiplier;
	bool isDeadly;
	bool isCheckpoint;
} TileEffect;

typedef enum
{
	RES_800x600,
	RES_1024x768,
	RES_1280x720,
	RES_1920x1080,
	RES_COUNT
} ResolutionMode;

// TODO:
// Heal mechanics
// Some kind of stupid ice ultimate attack spell card to clear bullets
typedef struct
{
	int moveLeft;
	int moveRight;
	int jump;
	int dash;
	int wallCling;
	int floatKey;
	int pause;
	int menuUp;
	int menuDown;
	int menuSelect;
	int menuBack;
} KeyBindings;

typedef struct
{
	bool fullscreen;
	bool soundEnabled;
	float masterVolume;
	ResolutionMode resolution;
	KeyBindings keys;
} Settings;
#endif
