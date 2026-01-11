#ifndef CONFIG_H
#define CONFIG_H
#include "raylib.h"
#include <stdbool.h>
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define TILE_SIZE 50
#define TARGET_FPS 60
#define MIN_WORLD_WIDTH 20
#define MIN_WORLD_HEIGHT 15
#define MAX_WORLD_WIDTH 128
#define MAX_WORLD_HEIGHT 128
#define PLAYER_SIZE 40
#define MAX_LEVELS 100
#define PLAYER_SPEED 250.0f
#define PLAYER_JUMP_SPEED 450.0f
#define GRAVITY 1200.0f
#define MAX_MUSIC_FILES 50
#define MAX_ENTITIES 100
#define MAX_SAVE_SLOTS 5
#define BASE_LEVEL_COUNT 9
#define PLAYER_MAX_HEALTH 3
#define BACKGROUND_TILE_START 20
typedef enum {
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
typedef enum {
  SPAWNER_PATTERN_CIRCLE,
  SPAWNER_PATTERN_SPIRAL,
  SPAWNER_PATTERN_WAVE,
  SPAWNER_PATTERN_BURST,
  SPAWNER_PATTERN_TARGETING
} SpawnerPattern;
typedef struct {
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
typedef struct {
  Vector2 position;
  Vector2 velocity;
  float radius;
  bool active;
} Bullet;
typedef struct {
  bool hasDamage;
  int damageAmount;
  bool hasJumpBoost;
  float jumpBoostMultiplier;
  bool isDeadly;
  bool isCheckpoint;
} TileEffect;
typedef enum {
  RES_800x600,
  RES_1024x768,
  RES_1280x720,
  RES_1920x1080,
  RES_COUNT
} ResolutionMode;
typedef struct {
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
typedef struct {
  bool fullscreen;
  bool soundEnabled;
  float masterVolume;
  ResolutionMode resolution;
  KeyBindings keys;
} Settings;
#endif
