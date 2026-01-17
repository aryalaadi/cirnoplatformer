#ifndef CONFIG_H
#define CONFIG_H
#include "raylib.h"
#include <stdbool.h>

//=============================================================================
// DISPLAY SETTINGS
//=============================================================================
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define TARGET_FPS 60

//=============================================================================
// TILE AND WORLD SETTINGS
//=============================================================================
#define TILE_SIZE 50
#define MIN_WORLD_WIDTH 20
#define MIN_WORLD_HEIGHT 15
#define MAX_WORLD_WIDTH 128
#define MAX_WORLD_HEIGHT 128
#define BACKGROUND_TILE_START 20

//=============================================================================
// PLAYER SETTINGS
//=============================================================================
#define PLAYER_SIZE 30
#define PLAYER_MAX_HEALTH 4

// Player Movement
#define PLAYER_SPEED 250.0f
#define PLAYER_JUMP_SPEED 350.0f
#define PLAYER_WALL_JUMP_SPEED_MULTIPLIER 1.5f
#define PLAYER_DASH_SPEED_MULTIPLIER 2.0f
#define PLAYER_FLOAT_SPEED_MULTIPLIER 0.5f
#define PLAYER_SLOWDOWN_MULTIPLIER 0.5f

// Player Timers
#define PLAYER_DASH_COOLDOWN 1.0f
#define PLAYER_DASH_DURATION 0.15f
#define PLAYER_FLOAT_DURATION 2.5f
#define PLAYER_FLOAT_COOLDOWN 4.0f
#define PLAYER_COYOTE_TIME 0.15f
#define PLAYER_JUMP_BUFFER_TIME 0.1f
#define PLAYER_WALL_JUMP_TIME 0.2f
#define PLAYER_WALL_CLING_DURATION 2.0f
#define PLAYER_INVULNERABILITY_TIME 1.5f
#define SPELLCARDTIME 5.0f

// Parry System
#define PARRY_WINDOW_TIME 0.4f         // Time window to successfully parry (in seconds)
#define PARRY_COOLDOWN 0.5f            // Cooldown between parry attempts
#define PARRY_ANGLE_THRESHOLD 90.0f    // Angle threshold in degrees (90 = semicircle)

// Player Animation Settings
#define PLAYER_ANIM_FRAME_DURATION 0.1f
#define PLAYER_SPRITE_FRAME_WIDTH 32
#define PLAYER_SPRITE_FRAME_HEIGHT 32

//=============================================================================
// PHYSICS SETTINGS
//=============================================================================
#define GRAVITY 1200.0f
#define WALL_SLIDE_GRAVITY_MULTIPLIER 0.3f
#define TERMINAL_VELOCITY 800.0f

//=============================================================================
// WORLD BOUNDS
//=============================================================================
#define OUT_OF_BOUNDS_MARGIN 200.0f  // Extra space below level before death
#define OUT_OF_BOUNDS_TOP_MARGIN 100.0f  // Extra space above level
#define OUT_OF_BOUNDS_SIDE_MARGIN 100.0f  // Extra space on sides

//=============================================================================
// GAME PROGRESSION
//=============================================================================
#define MAX_LEVELS 100
#define BASE_LEVEL_COUNT 9
#define BASE_WORLD_COUNT 9
#define LEVELS_PER_WORLD 9
#define MAX_CUSTOM_WORLDS 100
#define MAX_SAVE_SLOTS 5


//=============================================================================
// ASSET CONFIGS
//=============================================================================
#define ASSET_LEVEL_PATH "asset/levels/%s.lvl"
#define ASSET_PLAYER_PATH "asset/sprites/player.png"

//=============================================================================
// ENTITY LIMITS
//=============================================================================
#define MAX_ENTITIES 100
#define MAX_MUSIC_FILES 50
#define MAX_BULLETS 500
#define MAX_SPAWNERS 50
#define MAX_COLLECTIBLES 200
#define MAX_PARRY_EFFECTS 50

//=============================================================================
// PARRY EFFECT SYSTEM
//=============================================================================
#define PARRY_EFFECT_DURATION 0.3f      // How long the parry effect lasts
#define PARRY_EFFECT_RADIUS 20.0f       // Size of parry effect
#define PARRY_EFFECT_COLOR (Color){100, 200, 255, 200}  // Cyan-ish color

//=============================================================================
// SPAWNER SYSTEM
//=============================================================================
#define SPAWNER_INITIAL_HEALTH 5        // How many parried bullets to destroy spawner
#define PARRIED_BULLET_SPEED_MULTIPLIER 2.5f  // Speed multiplier for reflected bullets

//=============================================================================
// COLLECTIBLE SYSTEM
//=============================================================================
// Health Point System
#define HEALTH_POINTS_PER_HEAL 3
#define HEALTH_POINT_VALUE 1
#define HEALTH_POINT_TO_SCORE_MULTIPLIER 50
#define HEALTH_POINT_SIZE 8.0f
#define HEALTH_POINT_COLOR (Color){255, 100, 100, 255}

// Score System
#define SCORE_ITEM_VALUE 100
#define SCORE_ITEM_SIZE 10.0f
#define SCORE_ITEM_COLOR (Color){255, 215, 0, 255}

// Collectible Spawning
#define COLLECTIBLE_SPAWN_CHANCE 0.25f  
#define HEALTH_POINT_SPAWN_WEIGHT 0.5f  
#define COLLECTIBLE_SPEED 150.0f
#define COLLECTIBLE_LIFETIME 10.0f

//=============================================================================
// UI SETTINGS
//=============================================================================
#define UI_PAUSE_MENU_X (SCREEN_WIDTH / 2 - 100)
#define UI_PAUSE_MENU_Y 200
#define UI_PAUSE_MENU_ITEM_HEIGHT 40
#define UI_PAUSE_MENU_FONT_SIZE 28

#define UI_MENU_TITLE_Y 80
#define UI_MENU_TITLE_FONT_SIZE 30
#define UI_MENU_ITEM_START_Y 200
#define UI_MENU_ITEM_HEIGHT 40
#define UI_MENU_ITEM_FONT_SIZE 28

#define UI_HEALTH_X 20
#define UI_HEALTH_Y 20
#define UI_HEALTH_HEART_SIZE 30
#define UI_HEALTH_HEART_SPACING 10

#define UI_TIMER_X (SCREEN_WIDTH - 150)
#define UI_TIMER_Y 20
#define UI_TIMER_FONT_SIZE 24

#define UI_DEATH_COUNT_X 20
#define UI_DEATH_COUNT_Y (SCREEN_HEIGHT - 50)
#define UI_DEATH_COUNT_FONT_SIZE 20

//=============================================================================
// EDITOR SETTINGS
//=============================================================================
#define EDITOR_TILE_SCROLL_SPEED 50.0f
#define EDITOR_CAMERA_SPEED 400.0f
#define EDITOR_STATUS_MESSAGE_DURATION 3.0f
#define EDITOR_UI_PANEL_WIDTH 250
#define EDITOR_UI_PANEL_HEIGHT 600
#define EDITOR_TILE_PREVIEW_SIZE 40

//=============================================================================
// TIMING CONSTANTS
//=============================================================================
#define LEVEL_COMPLETE_DELAY 2.0f
#define DEATH_SCREEN_DELAY 1.5f
#define ACHIEVEMENT_NOTIFICATION_DURATION 5.0f
#define CREDITS_SCROLL_SPEED 50.0f

//=============================================================================
// TILE EFFECTS
//=============================================================================
#define TILE_JUMP_BOOST_MULTIPLIER 1.5f
#define TILE_DAMAGE_AMOUNT 1
#define TILE_SPIKE_DAMAGE_AMOUNT 1

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

typedef enum
{
	COLLECTIBLE_HEALTH_POINT,
	COLLECTIBLE_SCORE,
	COLLECTIBLE_COUNT
} CollectibleType;

typedef struct
{
	Vector2 position;
	Vector2 velocity;
	float radius;
	CollectibleType type;
	bool active;
	float lifetime;
} Collectible;

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
	int health;  // Spawner health - decreases when hit by parried bullets
} BulletSpawner;

typedef struct
{
	Vector2 position;
	Vector2 velocity;
	float radius;
	bool active;
	bool isParried;  // True if bullet was parried and is returning to spawner
	Color color;     // Color of the bullet
} Bullet;

typedef struct
{
	Vector2 position;
	float lifetime;
	float radius;
	bool active;
} ParryEffect;

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
	int slowDown;
	int heal;
	int pause;
    int spellcard;
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
