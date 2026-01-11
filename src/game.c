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

#include "game.h"
#include "menu.h"
#include "save.h"
#include "vn.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef _WIN32
#include <direct.h>
#define mkdir(dir, mode) _mkdir(dir)
#endif
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
typedef struct {
  Level level;
  Assets assets;
  Camera2D camera;
  int selectedTile;
  char levelName[256];
  char levelNameBuffer[256];
  int levelWidth;
  int levelHeight;
  int currentLevelIndex;
  int totalLevelFiles;
  char levelFiles[MAX_LEVELS][256];
  bool uiInteracting;
  bool needsRefresh;
  char statusMessage[256];
  float statusTimer;
  bool editingBackground;
  bool vnEditorOpen;
  int selectedDialogue;
  char vnCharNameBuffer[32];
  char vnTextBuffer[256];
  char vnSpriteBuffer[64];
  int vnBgColorIndex;
  bool vnEditingCharName;
  bool vnEditingText;
  bool vnEditingSprite;
  float vnScrollOffset;
  float tileScrollOffset;
} LevelEditor;
typedef enum { PAUSE_RESUME, PAUSE_SAVE, PAUSE_MENU, PAUSE_QUIT } PauseOption;
typedef enum { SAVE_MENU_NONE, SAVE_MENU_SAVE, SAVE_MENU_LOAD } SaveMenuState;
static GameState currentState;
static GameData gameData;
static World world;
static LevelEditor editor;
static VNState vnState;
static bool worldLoaded = false;
static bool editorLoaded = false;
static float levelCompleteTimer = 0;
static float deathScreenTimer = 0;
static float creditsScrollY = 0;
static Settings *settings = NULL;
static int pauseSelection = 0;
static int editorPauseSelection = 0;
static SaveMenuState saveMenuState = SAVE_MENU_NONE;
static int saveSlotSelection = 0;
static char playerNameInput[32] = "";
static int playerNameLength = 0;
static GameData saveSlots[MAX_SAVE_SLOTS];
static Texture2D menuBgDefault;
static Texture2D menuBgAllStages;
static Texture2D menuBgPerfect;
static bool menuBgLoaded = false;
static float achievementNotifTimer = 0;
static const float ACHIEVEMENT_NOTIF_DURATION = 5.0f;
void Game_Init(void) {
  mkdir("assets", 0777);
  mkdir("assets/levels", 0777);
  mkdir("assets/tiles", 0777);
  mkdir("assets/music", 0777);
  mkdir("assets/sounds", 0777);
  mkdir("saves", 0777);
  InitAudioDevice();
  Menu_Init();
  settings = Menu_GetSettings();
  currentState = STATE_MENU;
  worldLoaded = false;
  editorLoaded = false;
  Achievement_Init(&gameData.achievements);
  Achievement_Load(&gameData.achievements);
  if (!menuBgLoaded) {
    if (FileExists("assets/menu_bg_default.png")) {
      menuBgDefault = LoadTexture("assets/menu_bg_default.png");
    } else {
      Image img = GenImageGradientLinear(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
                                         DARKBLUE, SKYBLUE);
      menuBgDefault = LoadTextureFromImage(img);
      UnloadImage(img);
    }
    if (FileExists("assets/menu_bg_complete.png")) {
      menuBgAllStages = LoadTexture("assets/menu_bg_complete.png");
    } else {
      Image img = GenImageGradientLinear(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
                                         DARKPURPLE, PURPLE);
      menuBgAllStages = LoadTextureFromImage(img);
      UnloadImage(img);
    }
    if (FileExists("assets/menu_bg_perfect.png")) {
      menuBgPerfect = LoadTexture("assets/menu_bg_perfect.png");
    } else {
      Image img =
          GenImageGradientLinear(SCREEN_WIDTH, SCREEN_HEIGHT, 0, ORANGE, GOLD);
      menuBgPerfect = LoadTextureFromImage(img);
      UnloadImage(img);
    }
    menuBgLoaded = true;
  }
  SaveMetadata saveMeta[50];
  int saveCount = 0;
  Save_GetAllSaveFiles(saveMeta, 50, &saveCount);
  for (int i = 0; i < MAX_SAVE_SLOTS; i++) {
    saveSlots[i].isValid = false;
  }
}
void Editor_RefreshLevelList(void) {
  FilePathList files = LoadDirectoryFiles("assets/levels");
  editor.totalLevelFiles = 0;
  for (unsigned int i = 0;
       i < files.count && editor.totalLevelFiles < MAX_LEVELS; i++) {
    const char *ext = GetFileExtension(files.paths[i]);
    if (TextIsEqual(ext, ".lvl")) {
      strncpy(editor.levelFiles[editor.totalLevelFiles], files.paths[i], 255);
      editor.totalLevelFiles++;
    }
  }
  UnloadDirectoryFiles(files);
  editor.needsRefresh = false;
}
void Editor_NewLevel(void) {
  Level_Unload(&editor.level);
  Level_Create(&editor.level, 30, 20);
  strcpy(editor.levelName, "newlevel");
  strcpy(editor.levelNameBuffer, "newlevel");
  editor.currentLevelIndex = -1;
  strcpy(editor.statusMessage, "Created new level");
  editor.statusTimer = 3.0f;
}
void Editor_SaveLevel(void) {
  char filepath[512];
  snprintf(filepath, sizeof(filepath), "assets/levels/%s.lvl",
           editor.levelName);
  Level_SaveToFile(&editor.level, filepath);
  snprintf(editor.statusMessage, sizeof(editor.statusMessage), "Saved: %s",
           editor.levelName);
  editor.statusTimer = 3.0f;
  editor.needsRefresh = true;
}
void Editor_LoadLevel(int index) {
  if (index < 0 || index >= editor.totalLevelFiles)
    return;
  Level_Unload(&editor.level);
  Level_LoadFromFile(&editor.level, editor.levelFiles[index]);
  const char *fname = GetFileNameWithoutExt(editor.levelFiles[index]);
  strncpy(editor.levelName, fname, 255);
  strncpy(editor.levelNameBuffer, fname, 255);
  editor.currentLevelIndex = index;
  snprintf(editor.statusMessage, sizeof(editor.statusMessage), "Loaded: %s",
           editor.levelName);
  editor.statusTimer = 3.0f;
}
void Game_ShowSaveMenu(void) {
  saveMenuState = SAVE_MENU_SAVE;
  saveSlotSelection = 0;
}
void Game_ShowLoadMenu(void) {
  saveMenuState = SAVE_MENU_LOAD;
  saveSlotSelection = 0;
}
void Game_StartNew(void) {
  if (saveMenuState != SAVE_MENU_NONE) {
    saveMenuState = SAVE_MENU_NONE;
    return;
  }
  currentState = STATE_NAME_INPUT;
  playerNameLength = 0;
  memset(playerNameInput, 0, sizeof(playerNameInput));
}
void Game_StartNewWithName(void) {
  if (worldLoaded) {
    World_Unload(&world);
  }
  gameData.currentLevel = 0;
  gameData.totalLevels = BASE_LEVEL_COUNT;
  gameData.levelsCompleted = 0;
  gameData.deathCount = 0;
  gameData.health = PLAYER_MAX_HEALTH;
  memset(gameData.levelProgress, 0, sizeof(gameData.levelProgress));
  memset(gameData.levelDeaths, 0, sizeof(gameData.levelDeaths));
  snprintf(gameData.saveName, sizeof(gameData.saveName), "Save_%s",
           playerNameInput);
  strcpy(gameData.playerName, playerNameInput);
  gameData.isValid = true;
  World_Load(&world, gameData.currentLevel);
  worldLoaded = true;
  pauseSelection = 0;
  if (world.level.hasVisualNovel && world.level.dialogueCount > 0) {
    VN_Init(&vnState, &world.level);
    currentState = STATE_VISUAL_NOVEL;
  } else {
    currentState = STATE_PLAYING;
  }
  if (settings && settings->soundEnabled) {
    if (strlen(world.level.musicFile) > 0) {
      Assets_PlayMusic(&world.assets, world.level.musicFile);
    }
  }
}
void Game_LoadSave(void) {
  currentState = STATE_LOAD_GAME;
  saveSlotSelection = 0;
}
void Game_SaveProgress(void) {
  if (strlen(gameData.saveName) == 0) {
    snprintf(gameData.saveName, sizeof(gameData.saveName), "Save_%s",
             gameData.playerName);
  }
  char filename[256];
  snprintf(filename, sizeof(filename), "%s.sav", gameData.saveName);
  gameData.health = world.player.health;
  Achievement_Save(&gameData.achievements);
  Save_Write(&gameData, filename);
}
void Game_PauseToggle(void) {
  if (currentState == STATE_PLAYING) {
    currentState = STATE_PAUSED;
    pauseSelection = 0;
  } else if (currentState == STATE_PAUSED) {
    currentState = STATE_PLAYING;
  }
}
void Game_NextLevel(void) {
  if (worldLoaded) {
    World_Unload(&world);
  }
  gameData.currentLevel++;
  if (gameData.currentLevel >= gameData.totalLevels) {
    Achievement_CheckPerfectGame(&gameData.achievements, gameData.levelDeaths,
                                 gameData.totalLevels);
    if (gameData.achievements.newUnlock) {
      achievementNotifTimer = ACHIEVEMENT_NOTIF_DURATION;
      gameData.achievements.newUnlock = false;
    }
    Game_SaveProgress();
    currentState = STATE_CREDITS;
    creditsScrollY = SCREEN_HEIGHT;
    worldLoaded = false;
  } else {
    World_Load(&world, gameData.currentLevel);
    worldLoaded = true;
    if (world.level.hasVisualNovel && world.level.dialogueCount > 0) {
      VN_Init(&vnState, &world.level);
      currentState = STATE_VISUAL_NOVEL;
    } else {
      currentState = STATE_PLAYING;
    }
    if (settings && settings->soundEnabled) {
      if (strlen(world.level.musicFile) > 0) {
        Assets_PlayMusic(&world.assets, world.level.musicFile);
      }
    }
  }
}
void Game_StartLevelEditor(void) {
  if (!editorLoaded) {
    Assets_Load(&editor.assets);
    Editor_RefreshLevelList();
    editor.currentLevelIndex = -1;
    editor.levelWidth = 30;
    editor.levelHeight = 20;
    if (editor.totalLevelFiles > 0) {
      Editor_LoadLevel(0);
    } else {
      Editor_NewLevel();
    }
    editor.selectedTile = TILE_GRASS;
    editor.uiInteracting = false;
    editor.needsRefresh = false;
    editor.statusMessage[0] = '\0';
    editor.statusTimer = 0;
    editor.editingBackground = false;
    editor.vnEditorOpen = false;
    editor.selectedDialogue = -1;
    memset(editor.vnCharNameBuffer, 0, sizeof(editor.vnCharNameBuffer));
    memset(editor.vnTextBuffer, 0, sizeof(editor.vnTextBuffer));
    memset(editor.vnSpriteBuffer, 0, sizeof(editor.vnSpriteBuffer));
    editor.vnBgColorIndex = 0;
    editor.vnEditingCharName = false;
    editor.vnEditingText = false;
    editor.vnEditingSprite = false;
    editor.vnScrollOffset = 0;
    editor.tileScrollOffset = 0;
    editor.camera.target = (Vector2){400, 300};
    editor.camera.offset = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    editor.camera.rotation = 0.0f;
    editor.camera.zoom = 1.0f;
    editorLoaded = true;
  }
  currentState = STATE_LEVEL_EDITOR;
  editorPauseSelection = 0;
}
void Game_Update(void) {
  float dt = GetFrameTime();
  if (achievementNotifTimer > 0) {
    achievementNotifTimer -= dt;
  }
  if (worldLoaded && settings && settings->soundEnabled) {
    Assets_UpdateMusic(&world.assets);
  }
  if (editorLoaded && settings && settings->soundEnabled) {
    Assets_UpdateMusic(&editor.assets);
  }
  if (currentState == STATE_MENU) {
    Menu_Update();
  } else if (currentState == STATE_NAME_INPUT) {
    if (IsKeyPressed(KEY_ESCAPE)) {
      currentState = STATE_MENU;
      playerNameLength = 0;
      memset(playerNameInput, 0, sizeof(playerNameInput));
    }
    int key = GetCharPressed();
    while (key > 0) {
      if ((key >= 32) && (key <= 125) && (playerNameLength < 30)) {
        playerNameInput[playerNameLength] = (char)key;
        playerNameLength++;
        playerNameInput[playerNameLength] = '\0';
      }
      key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE) && playerNameLength > 0) {
      playerNameLength--;
      playerNameInput[playerNameLength] = '\0';
    }
    if (IsKeyPressed(KEY_ENTER) && playerNameLength > 0) {
      Game_StartNewWithName();
    }
  } else if (currentState == STATE_LOAD_GAME) {
    SaveMetadata saves[50];
    int saveCount = 0;
    Save_GetAllSaveFiles(saves, 50, &saveCount);
    if (IsKeyPressed(KEY_DOWN) && saveCount > 0) {
      saveSlotSelection = (saveSlotSelection + 1) % saveCount;
    }
    if (IsKeyPressed(KEY_UP) && saveCount > 0) {
      saveSlotSelection = (saveSlotSelection + saveCount - 1) % saveCount;
    }
    if (IsKeyPressed(KEY_ENTER) && saveCount > 0) {
      char filename[256];
      snprintf(filename, sizeof(filename), "%s.sav",
               saves[saveSlotSelection].saveName);
      if (Save_Read(&gameData, filename)) {
        Achievement_FixPointers(&gameData.achievements);
        if (worldLoaded) {
          World_Unload(&world);
        }
        World_Load(&world, gameData.currentLevel);
        world.player.health = gameData.health;
        currentState = STATE_PLAYING;
        worldLoaded = true;
        if (settings && settings->soundEnabled) {
          if (strlen(world.level.musicFile) > 0) {
            Assets_PlayMusic(&world.assets, world.level.musicFile);
          }
        }
      }
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
      currentState = STATE_MENU;
    }
  } else if (currentState == STATE_VISUAL_NOVEL) {
    VN_Update(&vnState, dt);
    if (VN_IsComplete(&vnState)) {
      VN_Unload(&vnState);
      currentState = STATE_PLAYING;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
      VN_Skip(&vnState);
    }
  } else if (currentState == STATE_DEATH_SCREEN) {
    deathScreenTimer += dt;
    if (IsKeyPressed(KEY_SPACE) || deathScreenTimer > 2.0f) {
      Player_Init(&world.player, world.player.lastCheckpoint);
      currentState = STATE_PLAYING;
      Game_SaveProgress();
    }
  } else if (currentState == STATE_CREDITS) {
    creditsScrollY -= 50 * dt;
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_SPACE) ||
        creditsScrollY < -800) {
      currentState = STATE_MENU;
    }
  } else if (currentState == STATE_LEVEL_EDITOR) {
    if (editor.needsRefresh) {
      Editor_RefreshLevelList();
    }
    if (editor.statusTimer > 0) {
      editor.statusTimer -= dt;
    }
    if (IsKeyPressed(KEY_ESCAPE) && !editor.uiInteracting) {
      if (editor.vnEditorOpen) {
        editor.vnEditorOpen = false;
      } else {
        currentState = STATE_EDITOR_PAUSED;
      }
      return;
    }
    if (IsKeyPressed(KEY_B) && !editor.uiInteracting && !editor.vnEditorOpen) {
      editor.editingBackground = !editor.editingBackground;
      const char *msg =
          editor.editingBackground ? "Background Layer" : "Foreground Layer";
      strcpy(editor.statusMessage, msg);
      editor.statusTimer = 2.0f;
    }
    Vector2 mousePos = GetMousePosition();
    editor.uiInteracting = (mousePos.x > SCREEN_WIDTH - 260);
    if (!editor.uiInteracting) {
      Vector2 worldMousePos = GetScreenToWorld2D(mousePos, editor.camera);
      int tileX = (int)(worldMousePos.x / TILE_SIZE);
      int tileY = (int)(worldMousePos.y / TILE_SIZE);
      if (IsKeyDown(KEY_LEFT))
        editor.camera.target.x -= 400 * dt;
      if (IsKeyDown(KEY_RIGHT))
        editor.camera.target.x += 400 * dt;
      if (IsKeyDown(KEY_UP))
        editor.camera.target.y -= 400 * dt;
      if (IsKeyDown(KEY_DOWN))
        editor.camera.target.y += 400 * dt;
      if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        if (tileX >= 0 && tileX < editor.level.width && tileY >= 0 &&
            tileY < editor.level.height) {
          if (editor.editingBackground) {
            editor.level.backgroundTiles[tileY][tileX] = editor.selectedTile;
          } else {
            Level_SetTile(&editor.level, tileX, tileY, editor.selectedTile);
          }
        }
      }
      if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
        if (tileX >= 0 && tileX < editor.level.width && tileY >= 0 &&
            tileY < editor.level.height) {
          if (editor.editingBackground) {
            editor.level.backgroundTiles[tileY][tileX] = TILE_EMPTY;
          } else {
            Level_SetTile(&editor.level, tileX, tileY, TILE_EMPTY);
          }
        }
      }
    }
  } else if (currentState == STATE_EDITOR_PAUSED) {
    if (IsKeyPressed(KEY_DOWN))
      editorPauseSelection = (editorPauseSelection + 1) % 3;
    if (IsKeyPressed(KEY_UP))
      editorPauseSelection = (editorPauseSelection + 2) % 3;
    if (IsKeyPressed(KEY_ENTER)) {
      if (editorPauseSelection == 0) {
        currentState = STATE_LEVEL_EDITOR;
      } else if (editorPauseSelection == 1) {
        Editor_SaveLevel();
        currentState = STATE_LEVEL_EDITOR;
      } else if (editorPauseSelection == 2) {
        currentState = STATE_MENU;
      }
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
      currentState = STATE_LEVEL_EDITOR;
    }
  } else if (currentState == STATE_PLAYING) {
    if (IsKeyPressed(settings->keys.pause)) {
      Game_PauseToggle();
      return;
    }
    World_Update(&world, dt, &settings->keys);
    if (!Player_IsAlive(&world.player)) {
      gameData.deathCount++;
      gameData.levelDeaths[gameData.currentLevel]++;
      deathScreenTimer = 0;
      currentState = STATE_DEATH_SCREEN;
    }
    if (World_LevelCompleted(&world)) {
      if (!gameData.levelProgress[gameData.currentLevel]) {
        gameData.levelProgress[gameData.currentLevel] = true;
        gameData.levelsCompleted++;
      }
      Achievement_CheckStageComplete(
          &gameData.achievements, gameData.currentLevel,
          gameData.levelDeaths[gameData.currentLevel]);
      if (gameData.achievements.newUnlock) {
        achievementNotifTimer = ACHIEVEMENT_NOTIF_DURATION;
        gameData.achievements.newUnlock = false;
      }
      gameData.currentLevel++;
      Game_SaveProgress();
      gameData.currentLevel--;
      Assets_PlayLevelCompleteSound(&world.assets);
      currentState = STATE_LEVEL_COMPLETE;
      levelCompleteTimer = 0;
    }
  } else if (currentState == STATE_PAUSED) {
    if (IsKeyPressed(settings->keys.menuDown) || IsKeyPressed(KEY_DOWN))
      pauseSelection = (pauseSelection + 1) % 3;
    if (IsKeyPressed(settings->keys.menuUp) || IsKeyPressed(KEY_UP))
      pauseSelection = (pauseSelection + 2) % 3;
    if (IsKeyPressed(settings->keys.menuSelect) || IsKeyPressed(KEY_ENTER)) {
      if (pauseSelection == 0) {
        Game_PauseToggle();
      } else if (pauseSelection == 1) {
        Game_SaveProgress();
      } else if (pauseSelection == 2) {
        if (worldLoaded) {
          Assets_StopMusic(&world.assets);
          World_Unload(&world);
          worldLoaded = false;
        }
        currentState = STATE_MENU;
      }
    }
    if (IsKeyPressed(settings->keys.pause) || IsKeyPressed(KEY_ESCAPE)) {
      Game_PauseToggle();
    }
  } else if (currentState == STATE_LEVEL_COMPLETE) {
    levelCompleteTimer += dt;
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) ||
        levelCompleteTimer > 3.0f) {
      Game_NextLevel();
    }
  } else if (currentState == STATE_GAME_COMPLETE) {
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
      currentState = STATE_MENU;
    }
  }
  if (saveMenuState == SAVE_MENU_SAVE || saveMenuState == SAVE_MENU_LOAD) {
    if (IsKeyPressed(KEY_DOWN))
      saveSlotSelection = (saveSlotSelection + 1) % MAX_SAVE_SLOTS;
    if (IsKeyPressed(KEY_UP))
      saveSlotSelection =
          (saveSlotSelection + MAX_SAVE_SLOTS - 1) % MAX_SAVE_SLOTS;
    if (IsKeyPressed(KEY_ENTER)) {
      if (saveMenuState == SAVE_MENU_SAVE) {
        Game_SaveProgress();
        saveMenuState = SAVE_MENU_NONE;
        currentState = STATE_PAUSED;
      } else {
        Game_LoadSave();
      }
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
      saveMenuState = SAVE_MENU_NONE;
      if (currentState == STATE_PAUSED) {
        currentState = STATE_PAUSED;
      } else {
        currentState = STATE_MENU;
      }
    }
  }
}
void Game_Draw(void) {
  if (currentState == STATE_MENU) {
    Texture2D bgToUse = menuBgDefault;
    if (gameData.isValid && gameData.achievements.unlockedCount > 0) {
      if (Achievement_IsUnlocked(&gameData.achievements, ACH_PERFECT_GAME)) {
        bgToUse = menuBgPerfect;
      } else {
        bool allStagesComplete = true;
        for (int i = ACH_COMPLETE_STAGE_1; i <= ACH_COMPLETE_STAGE_9; i++) {
          if (!Achievement_IsUnlocked(&gameData.achievements,
                                      (AchievementID)i)) {
            allStagesComplete = false;
            break;
          }
        }
        if (allStagesComplete) {
          bgToUse = menuBgAllStages;
        }
      }
    }
    DrawTexture(bgToUse, 0, 0, WHITE);
    Menu_Draw();
  } else if (currentState == STATE_NAME_INPUT) {
    ClearBackground((Color){20, 20, 40, 255});
    DrawText("CIRNO'S HARDEST PARKOUR", SCREEN_WIDTH / 2 - 250, 100, 40,
             SKYBLUE);
    DrawText("Enter Your Name", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 100,
             40, GOLD);
    DrawRectangle(SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2, 300, 50, DARKGRAY);
    DrawRectangleLines(SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2, 300, 50,
                       GOLD);
    DrawText(playerNameInput, SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 2 + 10,
             30, WHITE);
    if (((int)(GetTime() * 2)) % 2 == 0 && playerNameLength < 30) {
      DrawText("_", SCREEN_WIDTH / 2 - 140 + MeasureText(playerNameInput, 30),
               SCREEN_HEIGHT / 2 + 10, 30, YELLOW);
    }
    DrawText("Press ENTER to continue", SCREEN_WIDTH / 2 - 120,
             SCREEN_HEIGHT / 2 + 100, 20, GRAY);
    DrawText("Press ESC to go back", SCREEN_WIDTH / 2 - 100,
             SCREEN_HEIGHT / 2 + 130, 16, DARKGRAY);
  } else if (currentState == STATE_LOAD_GAME) {
    ClearBackground((Color){20, 20, 40, 255});
    DrawText("LOAD GAME", SCREEN_WIDTH / 2 - 120, 80, 40, SKYBLUE);
    SaveMetadata saves[50];
    int saveCount = 0;
    Save_GetAllSaveFiles(saves, 50, &saveCount);
    if (saveCount == 0) {
      DrawText("No save files found", SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2,
               30, GRAY);
      DrawText("Press ESC to go back", SCREEN_WIDTH / 2 - 100,
               SCREEN_HEIGHT / 2 + 50, 20, DARKGRAY);
    } else {
      int startY = 180;
      int lineHeight = 80;
      int displayCount = (saveCount < 6) ? saveCount : 6;
      int scrollOffset = (saveSlotSelection >= 6) ? (saveSlotSelection - 5) : 0;
      for (int i = scrollOffset;
           i < scrollOffset + displayCount && i < saveCount; i++) {
        int yPos = startY + (i - scrollOffset) * lineHeight;
        Color boxColor = (i == saveSlotSelection) ? (Color){70, 70, 120, 255}
                                                  : (Color){40, 40, 70, 255};
        Color borderColor = (i == saveSlotSelection) ? YELLOW : GRAY;
        DrawRectangle(100, yPos, SCREEN_WIDTH - 200, 70, boxColor);
        DrawRectangleLines(100, yPos, SCREEN_WIDTH - 200, 70, borderColor);
        DrawText(saves[i].saveName, 120, yPos + 10, 24, WHITE);
        DrawText(TextFormat("Level: %d", saves[i].currentLevel + 1), 120,
                 yPos + 40, 18, GOLD);
        DrawText(TextFormat("Health: %d", saves[i].health), 300, yPos + 40, 18,
                 GREEN);
        DrawText(TextFormat("Deaths: %d", saves[i].deathCount), 450, yPos + 40,
                 18, RED);
      }
      DrawText("Use UP/DOWN to select, ENTER to load", SCREEN_WIDTH / 2 - 180,
               SCREEN_HEIGHT - 80, 20, GRAY);
      DrawText("Press ESC to go back", SCREEN_WIDTH / 2 - 100,
               SCREEN_HEIGHT - 50, 18, DARKGRAY);
      if (saveCount > 6) {
        DrawText(TextFormat("%d/%d", saveSlotSelection + 1, saveCount),
                 SCREEN_WIDTH - 150, 150, 20, YELLOW);
      }
    }
  } else if (currentState == STATE_VISUAL_NOVEL) {
    VN_Draw(&vnState);
  } else if (currentState == STATE_DEATH_SCREEN) {
    World_Draw(&world);
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});
    DrawText("YOU DIED", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 50,
             RED);
    DrawText(TextFormat("Deaths: %d", gameData.deathCount),
             SCREEN_WIDTH / 2 - 70, SCREEN_HEIGHT / 2 + 20, 30, WHITE);
    DrawText(TextFormat("Level %d Deaths: %d", gameData.currentLevel + 1,
                        gameData.levelDeaths[gameData.currentLevel]),
             SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 60, 20, GRAY);
    if (deathScreenTimer > 1.0f) {
      if (((int)(GetTime() * 2)) % 2 == 0) {
        DrawText("Press SPACE to respawn", SCREEN_WIDTH / 2 - 120,
                 SCREEN_HEIGHT - 100, 20, YELLOW);
      }
    }
  } else if (currentState == STATE_CREDITS) {
    ClearBackground(BLACK);
    int y = (int)creditsScrollY;
    DrawText("CONGRATULATIONS!", SCREEN_WIDTH / 2 - 200, y, 50, GOLD);
    y += 80;
    DrawText(TextFormat("You completed the game, %s!", gameData.playerName),
             SCREEN_WIDTH / 2 - 200, y, 25, WHITE);
    y += 60;
    DrawText(TextFormat("Total Deaths: %d", gameData.deathCount),
             SCREEN_WIDTH / 2 - 120, y, 30, RED);
    y += 100;
    DrawText("CREDITS", SCREEN_WIDTH / 2 - 70, y, 40, YELLOW);
    y += 60;
    DrawText("Team Fairy", SCREEN_WIDTH / 2 - 80, y, 30, WHITE);
    y += 50;
    DrawText("Lead Developer & Artist:", SCREEN_WIDTH / 2 - 140, y, 20, GRAY);
    y += 30;
    DrawText("Aaditya Aryal", SCREEN_WIDTH / 2 - 80, y, 25, GOLD);
    y += 60;
    DrawText("Music:", SCREEN_WIDTH / 2 - 30, y, 20, GRAY);
    y += 30;
    DrawText("Original: ZUN", SCREEN_WIDTH / 2 - 60, y, 20, WHITE);
    y += 25;
    DrawText("Remixed: Aaditya Aryal", SCREEN_WIDTH / 2 - 100, y, 20, WHITE);
    y += 60;
    DrawText("Level Design:", SCREEN_WIDTH / 2 - 70, y, 20, GRAY);
    y += 30;
    DrawText("Aaditya Aryal", SCREEN_WIDTH / 2 - 70, y, 20, WHITE);
    y += 80;
    DrawText("Thank you for playing!", SCREEN_WIDTH / 2 - 120, y, 25, SKYBLUE);
    y += 50;
    DrawText("Press SPACE or ESC to return to menu", SCREEN_WIDTH / 2 - 180, y,
             18, GRAY);
  } else if (currentState == STATE_LEVEL_EDITOR ||
             currentState == STATE_EDITOR_PAUSED) {
    ClearBackground((Color){50, 50, 70, 255});
    BeginMode2D(editor.camera);
    Level_Draw(&editor.level, &editor.assets, editor.camera);
    DrawCircle((int)editor.level.playerSpawn.x + TILE_SIZE / 2,
               (int)editor.level.playerSpawn.y + TILE_SIZE / 2, 20, BLUE);
    if (currentState == STATE_LEVEL_EDITOR && !editor.uiInteracting) {
      Vector2 worldMousePos =
          GetScreenToWorld2D(GetMousePosition(), editor.camera);
      int tileX = (int)(worldMousePos.x / TILE_SIZE);
      int tileY = (int)(worldMousePos.y / TILE_SIZE);
      if (tileX >= 0 && tileX < editor.level.width && tileY >= 0 &&
          tileY < editor.level.height) {
        DrawRectangleLines(tileX * TILE_SIZE, tileY * TILE_SIZE, TILE_SIZE,
                           TILE_SIZE, YELLOW);
        DrawText(TextFormat("%d", editor.selectedTile), tileX * TILE_SIZE + 5,
                 tileY * TILE_SIZE + 5, 16, YELLOW);
        if (editor.selectedTile == TILE_SPAWNER_CIRCLE) {
          DrawCircle(tileX * TILE_SIZE + 25, tileY * TILE_SIZE + 25, 15,
                     (Color){255, 0, 0, 100});
          DrawText("CIRCLE", tileX * TILE_SIZE + 5, tileY * TILE_SIZE - 20, 12,
                   RED);
        } else if (editor.selectedTile == TILE_SPAWNER_SPIRAL) {
          DrawCircle(tileX * TILE_SIZE + 25, tileY * TILE_SIZE + 25, 15,
                     (Color){128, 0, 128, 100});
          DrawText("SPIRAL", tileX * TILE_SIZE + 5, tileY * TILE_SIZE - 20, 12,
                   PURPLE);
        } else if (editor.selectedTile == TILE_SPAWNER_WAVE) {
          DrawCircle(tileX * TILE_SIZE + 25, tileY * TILE_SIZE + 25, 15,
                     (Color){0, 0, 255, 100});
          DrawText("WAVE", tileX * TILE_SIZE + 8, tileY * TILE_SIZE - 20, 12,
                   BLUE);
        } else if (editor.selectedTile == TILE_SPAWNER_BURST) {
          DrawCircle(tileX * TILE_SIZE + 25, tileY * TILE_SIZE + 25, 15,
                     (Color){255, 165, 0, 100});
          DrawText("BURST", tileX * TILE_SIZE + 5, tileY * TILE_SIZE - 20, 12,
                   ORANGE);
        }
      }
    }
    EndMode2D();
    DrawRectangle(SCREEN_WIDTH - 260, 0, 260, SCREEN_HEIGHT,
                  (Color){30, 30, 50, 255});
    DrawLine(SCREEN_WIDTH - 260, 0, SCREEN_WIDTH - 260, SCREEN_HEIGHT, YELLOW);
    int yPos = 10;
    GuiLabel((Rectangle){SCREEN_WIDTH - 250, yPos, 240, 25}, "LEVEL EDITOR");
    yPos += 30;
    GuiLine((Rectangle){SCREEN_WIDTH - 250, yPos, 240, 1}, NULL);
    yPos += 10;
    GuiLabel((Rectangle){SCREEN_WIDTH - 250, yPos, 240, 20}, "Level Name:");
    yPos += 25;
    Rectangle nameBoxRect = {SCREEN_WIDTH - 250, yPos, 240, 30};
    bool nameBoxClicked =
        CheckCollisionPointRec(GetMousePosition(), nameBoxRect) &&
        IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    if (nameBoxClicked) {
      editor.uiInteracting = true;
    }
    if (GuiTextBox(nameBoxRect, editor.levelNameBuffer, 255,
                   editor.uiInteracting)) {
      strcpy(editor.levelName, editor.levelNameBuffer);
    }
    if (editor.uiInteracting && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        !CheckCollisionPointRec(GetMousePosition(), nameBoxRect)) {
      editor.uiInteracting = false;
    }
    yPos += 40;
    if (GuiButton((Rectangle){SCREEN_WIDTH - 250, yPos, 115, 30},
                  "New Level")) {
      Editor_NewLevel();
    }
    if (GuiButton((Rectangle){SCREEN_WIDTH - 130, yPos, 115, 30},
                  "Save Level")) {
      Editor_SaveLevel();
    }
    yPos += 40;
    GuiLine((Rectangle){SCREEN_WIDTH - 250, yPos, 240, 1}, NULL);
    yPos += 10;
    GuiLabel((Rectangle){SCREEN_WIDTH - 250, yPos, 240, 20},
             TextFormat("Levels: %d", editor.totalLevelFiles));
    yPos += 25;
    if (GuiButton((Rectangle){SCREEN_WIDTH - 250, yPos, 115, 30},
                  "< Previous")) {
      if (editor.currentLevelIndex > 0) {
        Editor_LoadLevel(editor.currentLevelIndex - 1);
      }
    }
    if (GuiButton((Rectangle){SCREEN_WIDTH - 130, yPos, 115, 30}, "Next >")) {
      if (editor.currentLevelIndex < editor.totalLevelFiles - 1) {
        Editor_LoadLevel(editor.currentLevelIndex + 1);
      }
    }
    yPos += 35;
    if (GuiButton((Rectangle){SCREEN_WIDTH - 250, yPos, 240, 25},
                  "Refresh List")) {
      Editor_RefreshLevelList();
    }
    yPos += 35;
    if (GuiButton((Rectangle){SCREEN_WIDTH - 250, yPos, 240, 30},
                  TextFormat("VN Editor (%d)", editor.level.dialogueCount))) {
      editor.vnEditorOpen = !editor.vnEditorOpen;
    }
    yPos += 35;
    GuiLine((Rectangle){SCREEN_WIDTH - 250, yPos, 240, 1}, NULL);
    yPos += 10;
    const char *layerText =
        editor.editingBackground ? "Layer: BACKGROUND" : "Layer: FOREGROUND";
    Color layerColor = editor.editingBackground ? SKYBLUE : YELLOW;
    DrawText(layerText, SCREEN_WIDTH - 245, yPos, 16, layerColor);
    DrawText("(Press B to toggle)", SCREEN_WIDTH - 245, yPos + 18, 12, GRAY);
    yPos += 45;
    GuiLine((Rectangle){SCREEN_WIDTH - 250, yPos, 240, 1}, NULL);
    yPos += 10;
    GuiLabel((Rectangle){SCREEN_WIDTH - 250, yPos, 240, 20}, "TILE PALETTE");
    yPos += 25;
    int paletteStartY = yPos;
    int paletteHeight = 400;
    int paletteX = SCREEN_WIDTH - 250;
    int paletteW = 240;
    DrawRectangle(paletteX, paletteStartY, paletteW, paletteHeight,
                  (Color){20, 20, 35, 255});
    DrawRectangleLinesEx(
        (Rectangle){paletteX, paletteStartY, paletteW, paletteHeight}, 2,
        DARKGRAY);
    Vector2 mousePos = GetMousePosition();
    Rectangle paletteRect = {paletteX, paletteStartY, paletteW, paletteHeight};
    if (CheckCollisionPointRec(mousePos, paletteRect)) {
      float wheel = GetMouseWheelMove();
      editor.tileScrollOffset -= wheel * 40;
      if (editor.tileScrollOffset < 0)
        editor.tileScrollOffset = 0;
    }
    BeginScissorMode(paletteX, paletteStartY, paletteW, paletteHeight);
    int tileY = paletteStartY + 5 - (int)editor.tileScrollOffset;
    int tilesPerRow = 4;
    int tileSpacing = 5;
    int buttonSize = (paletteW - (tilesPerRow + 1) * tileSpacing) / tilesPerRow;
    Rectangle hoveredTileRect = {0};
    int hoveredTileId = -1;
    const char *hoveredTileName = NULL;
    for (int i = 0; i < 20; i++) {
      int col = i % tilesPerRow;
      int row = i / tilesPerRow;
      int btnX = paletteX + tileSpacing + col * (buttonSize + tileSpacing);
      int btnY = tileY + row * (buttonSize + tileSpacing);
      Rectangle btnRect = {btnX, btnY, buttonSize, buttonSize};
      if (btnY + buttonSize > paletteStartY &&
          btnY < paletteStartY + paletteHeight) {
        bool isHovered = CheckCollisionPointRec(mousePos, btnRect) &&
                         CheckCollisionPointRec(mousePos, paletteRect);
        bool isSelected =
            (i == editor.selectedTile && !editor.editingBackground);
        Color btnColor =
            isSelected ? (Color){80, 80, 120, 255} : (Color){40, 40, 60, 255};
        if (isHovered)
          btnColor = (Color){60, 60, 90, 255};
        DrawRectangleRec(btnRect, btnColor);
        Rectangle tileSource = Assets_GetTileSource(i);
        if (tileSource.width > 0) {
          Rectangle tileDest = {btnX + 2, btnY + 2, buttonSize - 4,
                                buttonSize - 4};
          DrawTexturePro(editor.assets.tileset, tileSource, tileDest,
                         (Vector2){0, 0}, 0, WHITE);
        } else {
          for (int gx = 0; gx < 4; gx++) {
            for (int gy = 0; gy < 4; gy++) {
              if ((gx + gy) % 2 == 0) {
                DrawRectangle(btnX + 2 + gx * 9, btnY + 2 + gy * 9, 9, 9,
                              (Color){60, 60, 80, 255});
              }
            }
          }
        }
        DrawRectangleLinesEx(btnRect, isSelected ? 3 : 1,
                             isSelected ? GOLD : DARKGRAY);
        DrawText(TextFormat("%d", i), btnX + 3, btnY + 3, 10, YELLOW);
        if (isHovered) {
          hoveredTileRect = btnRect;
          hoveredTileId = i;
          switch (i) {
          case 0:
            hoveredTileName = "Empty";
            break;
          case 1:
            hoveredTileName = "Grass";
            break;
          case 2:
            hoveredTileName = "Dirt";
            break;
          case 3:
            hoveredTileName = "Stone";
            break;
          case 4:
            hoveredTileName = "Goal";
            break;
          case 5:
            hoveredTileName = "Damage";
            break;
          case 6:
            hoveredTileName = "Jump Boost";
            break;
          case 7:
            hoveredTileName = "Spike";
            break;
          case 8:
            hoveredTileName = "Checkpoint";
            break;
          case 9:
            hoveredTileName = "Spawner Circle";
            break;
          case 10:
            hoveredTileName = "Spawner Spiral";
            break;
          case 11:
            hoveredTileName = "Spawner Wave";
            break;
          case 12:
            hoveredTileName = "Spawner Burst";
            break;
          default:
            hoveredTileName = TextFormat("Tile %d", i);
            break;
          }
        }
        if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
          editor.selectedTile = i;
          editor.editingBackground = false;
        }
      }
    }
    int separatorY =
        tileY + ((19 / tilesPerRow) + 1) * (buttonSize + tileSpacing) + 10;
    if (separatorY > paletteStartY &&
        separatorY < paletteStartY + paletteHeight) {
      DrawText("BACKGROUND TILES (20+)", paletteX + 10, separatorY, 12,
               SKYBLUE);
    }
    separatorY += 20;
    for (int i = 0; i < 80; i++) {
      int tileId = BACKGROUND_TILE_START + i;
      int col = i % tilesPerRow;
      int row = i / tilesPerRow;
      int btnX = paletteX + tileSpacing + col * (buttonSize + tileSpacing);
      int btnY = separatorY + row * (buttonSize + tileSpacing);
      Rectangle btnRect = {btnX, btnY, buttonSize, buttonSize};
      if (btnY + buttonSize > paletteStartY &&
          btnY < paletteStartY + paletteHeight) {
        bool isHovered = CheckCollisionPointRec(mousePos, btnRect) &&
                         CheckCollisionPointRec(mousePos, paletteRect);
        bool isSelected =
            (tileId == editor.selectedTile && editor.editingBackground);
        Color btnColor =
            isSelected ? (Color){80, 120, 120, 255} : (Color){40, 60, 60, 255};
        if (isHovered)
          btnColor = (Color){60, 90, 90, 255};
        DrawRectangleRec(btnRect, btnColor);
        Rectangle tileSource = Assets_GetTileSource(tileId);
        if (tileSource.width > 0) {
          Rectangle tileDest = {btnX + 2, btnY + 2, buttonSize - 4,
                                buttonSize - 4};
          DrawTexturePro(editor.assets.tileset, tileSource, tileDest,
                         (Vector2){0, 0}, 0, (Color){255, 255, 255, 180});
        } else {
          DrawText("BG", btnX + 10, btnY + 12, 12, SKYBLUE);
        }
        DrawRectangleLinesEx(btnRect, isSelected ? 3 : 1,
                             isSelected ? SKYBLUE : DARKGRAY);
        DrawText(TextFormat("%d", tileId), btnX + 3, btnY + 3, 10, SKYBLUE);
        if (isHovered) {
          hoveredTileRect = btnRect;
          hoveredTileId = tileId;
          hoveredTileName = TextFormat("Background %d", tileId);
        }
        if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
          editor.selectedTile = tileId;
          editor.editingBackground = true;
        }
      }
    }
    EndScissorMode();
    if (hoveredTileId >= 0 && hoveredTileName != NULL) {
      int tooltipX = (int)hoveredTileRect.x + (int)hoveredTileRect.width + 5;
      int tooltipY = (int)hoveredTileRect.y;
      int tooltipW = MeasureText(hoveredTileName, 14) + 10;
      int tooltipH = 22;
      if (tooltipX + tooltipW > SCREEN_WIDTH) {
        tooltipX = (int)hoveredTileRect.x - tooltipW - 5;
      }
      DrawRectangle(tooltipX, tooltipY, tooltipW, tooltipH,
                    (Color){0, 0, 0, 230});
      DrawRectangleLinesEx((Rectangle){tooltipX, tooltipY, tooltipW, tooltipH},
                           1, GOLD);
      DrawText(hoveredTileName, tooltipX + 5, tooltipY + 4, 14, WHITE);
    }
    int maxScroll = ((19 + 80) / tilesPerRow + 3) * (buttonSize + tileSpacing) +
                    40 - paletteHeight;
    if (maxScroll > 0) {
      float scrollPercent = editor.tileScrollOffset / maxScroll;
      if (scrollPercent > 1.0f)
        scrollPercent = 1.0f;
      int scrollbarHeight =
          (int)(paletteHeight *
                ((float)paletteHeight / (paletteHeight + maxScroll)));
      if (scrollbarHeight < 20)
        scrollbarHeight = 20;
      int scrollbarY = paletteStartY +
                       (int)((paletteHeight - scrollbarHeight) * scrollPercent);
      DrawRectangle(paletteX + paletteW - 8, scrollbarY, 6, scrollbarHeight,
                    GOLD);
    }
    yPos = paletteStartY + paletteHeight + 10;
    if (editor.statusTimer > 0) {
      DrawRectangle(0, SCREEN_HEIGHT - 40, SCREEN_WIDTH, 40,
                    (Color){20, 20, 20, 230});
      DrawText(editor.statusMessage, 10, SCREEN_HEIGHT - 28, 20, GREEN);
    }
    DrawRectangle(0, 0, SCREEN_WIDTH - 260, 60, (Color){0, 0, 0, 200});
    DrawText("CONTROLS:", 10, 5, 16, YELLOW);
    DrawText("Arrows: Camera | Click Palette: Select | L-Click: Place | "
             "R-Click: Erase",
             10, 23, 13, WHITE);
    DrawText("Spawners (9-12): C=Circle, S=Spiral, W=Wave, B=Burst | ESC: Menu",
             10, 40, 13, WHITE);
    DrawRectangle(SCREEN_WIDTH - 260, 0, 260, 30, (Color){0, 0, 0, 200});
    const char *GetTileName(int tile) {
      switch (tile) {
      case 0:
        return "Empty";
      case 1:
        return "Grass";
      case 2:
        return "Dirt";
      case 3:
        return "Stone";
      case 4:
        return "Goal";
      case 5:
        return "Damage";
      case 6:
        return "Jump Boost";
      case 7:
        return "Spike";
      case 8:
        return "Checkpoint";
      case 9:
        return "Spawner Circle";
      case 10:
        return "Spawner Spiral";
      case 11:
        return "Spawner Wave";
      case 12:
        return "Spawner Burst";
      default:
        return (tile >= BACKGROUND_TILE_START) ? "Background" : "Unknown";
      }
    }
    const char *layerPrefix = editor.editingBackground ? "BG: " : "";
    DrawText(TextFormat("Selected: %s%s (%d)", layerPrefix,
                        GetTileName(editor.selectedTile), editor.selectedTile),
             SCREEN_WIDTH - 250, 8, 16,
             editor.editingBackground ? SKYBLUE : GOLD);
    if (currentState == STATE_EDITOR_PAUSED) {
      DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 180});
      DrawText("EDITOR MENU", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 100,
               40, YELLOW);
      const char *editorOptions[] = {"Resume", "Save Level", "Exit to Menu"};
      for (int i = 0; i < 3; i++) {
        Color c = (i == editorPauseSelection) ? YELLOW : WHITE;
        DrawText(editorOptions[i], SCREEN_WIDTH / 2 - 80,
                 SCREEN_HEIGHT / 2 - 20 + i * 35, 24, c);
      }
    }
    if (editor.vnEditorOpen) {
      DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 220});
      int panelW = 700;
      int panelH = 500;
      int panelX = (SCREEN_WIDTH - panelW) / 2;
      int panelY = (SCREEN_HEIGHT - panelH) / 2;
      DrawRectangle(panelX, panelY, panelW, panelH, (Color){25, 25, 35, 255});
      DrawRectangleLinesEx((Rectangle){panelX, panelY, panelW, panelH}, 3,
                           GOLD);
      DrawText("VISUAL NOVEL EDITOR", panelX + 20, panelY + 15, 24, GOLD);
      DrawText("ESC to close", panelX + panelW - 120, panelY + 20, 14, GRAY);
      int y = panelY + 55;
      DrawText("Visual Novel:", panelX + 20, y, 18, WHITE);
      bool vnEnabled = editor.level.hasVisualNovel;
      if (GuiCheckBox((Rectangle){panelX + 150, y - 2, 20, 20}, "",
                      &vnEnabled)) {
        editor.level.hasVisualNovel = vnEnabled;
      }
      DrawText(vnEnabled ? "ENABLED" : "DISABLED", panelX + 180, y, 18,
               vnEnabled ? GREEN : RED);
      y += 35;
      if (editor.level.hasVisualNovel) {
        GuiLine((Rectangle){panelX + 20, y, panelW - 40, 1}, NULL);
        y += 10;
        int listX = panelX + 20;
        int listW = 220;
        DrawText(TextFormat("Dialogues (%d/20):", editor.level.dialogueCount),
                 listX, y, 16, YELLOW);
        if (GuiButton((Rectangle){listX + 150, y - 3, 70, 25}, "+ Add")) {
          if (editor.level.dialogueCount < 20) {
            editor.selectedDialogue = editor.level.dialogueCount;
            strcpy(editor.level.dialogues[editor.level.dialogueCount]
                       .characterName,
                   "Character");
            strcpy(editor.level.dialogues[editor.level.dialogueCount].text,
                   "Dialogue text here");
            editor.level.dialogues[editor.level.dialogueCount]
                .characterSprite[0] = '\0';
            editor.level.dialogues[editor.level.dialogueCount].bgColor = 0;
            editor.level.dialogueCount++;
            strcpy(
                editor.vnCharNameBuffer,
                editor.level.dialogues[editor.selectedDialogue].characterName);
            strcpy(editor.vnTextBuffer,
                   editor.level.dialogues[editor.selectedDialogue].text);
            strcpy(editor.vnSpriteBuffer, "");
            editor.vnBgColorIndex = 0;
          }
        }
        y += 30;
        DrawRectangle(listX, y, listW, 320, (Color){15, 15, 25, 255});
        DrawRectangleLinesEx((Rectangle){listX, y, listW, 320}, 2, DARKGRAY);
        for (int i = 0; i < editor.level.dialogueCount; i++) {
          Color btnColor = (i == editor.selectedDialogue)
                               ? (Color){60, 60, 80, 255}
                               : (Color){30, 30, 45, 255};
          Rectangle btnRect = {listX + 5, y + 5 + i * 32, listW - 10, 28};
          DrawRectangleRec(btnRect, btnColor);
          if (i == editor.selectedDialogue) {
            DrawRectangleLinesEx(btnRect, 2, GOLD);
          }
          if (CheckCollisionPointRec(GetMousePosition(), btnRect) &&
              IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            editor.selectedDialogue = i;
            strcpy(editor.vnCharNameBuffer,
                   editor.level.dialogues[i].characterName);
            strcpy(editor.vnTextBuffer, editor.level.dialogues[i].text);
            strcpy(editor.vnSpriteBuffer,
                   editor.level.dialogues[i].characterSprite);
            editor.vnBgColorIndex = editor.level.dialogues[i].bgColor;
            editor.vnEditingCharName = false;
            editor.vnEditingText = false;
            editor.vnEditingSprite = false;
          }
          DrawText(TextFormat("#%d: %s", i + 1,
                              editor.level.dialogues[i].characterName),
                   listX + 10, y + 12 + i * 32, 14, WHITE);
        }
        if (editor.selectedDialogue >= 0 &&
            editor.selectedDialogue < editor.level.dialogueCount) {
          int editX = listX + listW + 30;
          int editY = y;
          int editW = 400;
          DrawText(
              TextFormat("Editing Dialogue #%d", editor.selectedDialogue + 1),
              editX, editY, 18, SKYBLUE);
          editY += 28;
          DrawText("Character Name:", editX, editY, 15, WHITE);
          editY += 20;
          Rectangle nameBox = {editX, editY, editW, 30};
          DrawRectangleRec(nameBox, editor.vnEditingCharName
                                        ? (Color){40, 40, 60, 255}
                                        : (Color){30, 30, 45, 255});
          DrawRectangleLinesEx(nameBox, 2,
                               editor.vnEditingCharName ? SKYBLUE : DARKGRAY);
          if (CheckCollisionPointRec(GetMousePosition(), nameBox) &&
              IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            editor.vnEditingCharName = true;
            editor.vnEditingText = false;
            editor.vnEditingSprite = false;
          }
          if (editor.vnEditingCharName) {
            int key = GetCharPressed();
            while (key > 0) {
              if (key >= 32 && key <= 125 &&
                  strlen(editor.vnCharNameBuffer) < 30) {
                int len = strlen(editor.vnCharNameBuffer);
                editor.vnCharNameBuffer[len] = (char)key;
                editor.vnCharNameBuffer[len + 1] = '\0';
                strcpy(editor.level.dialogues[editor.selectedDialogue]
                           .characterName,
                       editor.vnCharNameBuffer);
              }
              key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) &&
                strlen(editor.vnCharNameBuffer) > 0) {
              editor.vnCharNameBuffer[strlen(editor.vnCharNameBuffer) - 1] =
                  '\0';
              strcpy(
                  editor.level.dialogues[editor.selectedDialogue].characterName,
                  editor.vnCharNameBuffer);
            }
          }
          DrawText(editor.vnCharNameBuffer, editX + 5, editY + 8, 16, WHITE);
          if (editor.vnEditingCharName && ((int)(GetTime() * 2) % 2 == 0)) {
            DrawText("_", editX + 5 + MeasureText(editor.vnCharNameBuffer, 16),
                     editY + 8, 16, YELLOW);
          }
          editY += 38;
          DrawText("Dialogue Text:", editX, editY, 15, WHITE);
          editY += 20;
          Rectangle textBox = {editX, editY, editW, 80};
          DrawRectangleRec(textBox, editor.vnEditingText
                                        ? (Color){40, 40, 60, 255}
                                        : (Color){30, 30, 45, 255});
          DrawRectangleLinesEx(textBox, 2,
                               editor.vnEditingText ? SKYBLUE : DARKGRAY);
          if (CheckCollisionPointRec(GetMousePosition(), textBox) &&
              IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            editor.vnEditingText = true;
            editor.vnEditingCharName = false;
            editor.vnEditingSprite = false;
          }
          if (editor.vnEditingText) {
            int key = GetCharPressed();
            while (key > 0) {
              if (key >= 32 && key <= 125 &&
                  strlen(editor.vnTextBuffer) < 254) {
                int len = strlen(editor.vnTextBuffer);
                editor.vnTextBuffer[len] = (char)key;
                editor.vnTextBuffer[len + 1] = '\0';
                strcpy(editor.level.dialogues[editor.selectedDialogue].text,
                       editor.vnTextBuffer);
              }
              key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) &&
                strlen(editor.vnTextBuffer) > 0) {
              editor.vnTextBuffer[strlen(editor.vnTextBuffer) - 1] = '\0';
              strcpy(editor.level.dialogues[editor.selectedDialogue].text,
                     editor.vnTextBuffer);
            }
            if (IsKeyPressed(KEY_ENTER) && strlen(editor.vnTextBuffer) < 253) {
              strcat(editor.vnTextBuffer, " ");
              strcpy(editor.level.dialogues[editor.selectedDialogue].text,
                     editor.vnTextBuffer);
            }
          }
          DrawText(editor.vnTextBuffer, editX + 5, editY + 5, 14, WHITE);
          if (editor.vnEditingText && ((int)(GetTime() * 2) % 2 == 0)) {
            DrawText("_", editX + 5 + MeasureText(editor.vnTextBuffer, 14),
                     editY + 5, 14, YELLOW);
          }
          editY += 88;
          DrawText("Sprite Path (optional):", editX, editY, 15, WHITE);
          editY += 20;
          Rectangle spriteBox = {editX, editY, editW, 28};
          DrawRectangleRec(spriteBox, editor.vnEditingSprite
                                          ? (Color){40, 40, 60, 255}
                                          : (Color){30, 30, 45, 255});
          DrawRectangleLinesEx(spriteBox, 2,
                               editor.vnEditingSprite ? SKYBLUE : DARKGRAY);
          if (CheckCollisionPointRec(GetMousePosition(), spriteBox) &&
              IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            editor.vnEditingSprite = true;
            editor.vnEditingCharName = false;
            editor.vnEditingText = false;
          }
          if (editor.vnEditingSprite) {
            int key = GetCharPressed();
            while (key > 0) {
              if (key >= 32 && key <= 125 &&
                  strlen(editor.vnSpriteBuffer) < 62) {
                int len = strlen(editor.vnSpriteBuffer);
                editor.vnSpriteBuffer[len] = (char)key;
                editor.vnSpriteBuffer[len + 1] = '\0';
                strcpy(editor.level.dialogues[editor.selectedDialogue]
                           .characterSprite,
                       editor.vnSpriteBuffer);
              }
              key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) &&
                strlen(editor.vnSpriteBuffer) > 0) {
              editor.vnSpriteBuffer[strlen(editor.vnSpriteBuffer) - 1] = '\0';
              strcpy(editor.level.dialogues[editor.selectedDialogue]
                         .characterSprite,
                     editor.vnSpriteBuffer);
            }
          }
          DrawText(editor.vnSpriteBuffer[0] ? editor.vnSpriteBuffer
                                            : "No sprite",
                   editX + 5, editY + 6, 14,
                   editor.vnSpriteBuffer[0] ? WHITE : GRAY);
          if (editor.vnEditingSprite && ((int)(GetTime() * 2) % 2 == 0)) {
            DrawText("_", editX + 5 + MeasureText(editor.vnSpriteBuffer, 14),
                     editY + 6, 14, YELLOW);
          }
          editY += 35;
          DrawText("Background:", editX, editY, 15, WHITE);
          const char *colorNames[] = {"Dark Blue", "Dark Red", "Dark Green",
                                      "Gray", "Black"};
          DrawText(colorNames[editor.vnBgColorIndex], editX + 110, editY, 15,
                   SKYBLUE);
          if (GuiButton((Rectangle){editX + 230, editY - 3, 30, 25}, "<")) {
            editor.vnBgColorIndex = (editor.vnBgColorIndex - 1 + 5) % 5;
            editor.level.dialogues[editor.selectedDialogue].bgColor =
                editor.vnBgColorIndex;
          }
          if (GuiButton((Rectangle){editX + 265, editY - 3, 30, 25}, ">")) {
            editor.vnBgColorIndex = (editor.vnBgColorIndex + 1) % 5;
            editor.level.dialogues[editor.selectedDialogue].bgColor =
                editor.vnBgColorIndex;
          }
          editY += 35;
          if (GuiButton((Rectangle){editX, editY, 160, 32},
                        "Delete Dialogue")) {
            for (int i = editor.selectedDialogue;
                 i < editor.level.dialogueCount - 1; i++) {
              editor.level.dialogues[i] = editor.level.dialogues[i + 1];
            }
            editor.level.dialogueCount--;
            if (editor.selectedDialogue >= editor.level.dialogueCount) {
              editor.selectedDialogue = editor.level.dialogueCount - 1;
            }
            if (editor.selectedDialogue >= 0) {
              strcpy(editor.vnCharNameBuffer,
                     editor.level.dialogues[editor.selectedDialogue]
                         .characterName);
              strcpy(editor.vnTextBuffer,
                     editor.level.dialogues[editor.selectedDialogue].text);
            }
          }
        }
        DrawText("Click text boxes to edit | This VN plays BEFORE the level",
                 panelX + 20, panelY + panelH - 30, 13, YELLOW);
      }
    }
  } else if (currentState == STATE_PLAYING || currentState == STATE_PAUSED) {
    World_Draw(&world);
    DrawRectangle(0, 0, 280, 115, (Color){0, 0, 0, 150});
    DrawText(TextFormat("Level: %d/%d", gameData.currentLevel + 1,
                        gameData.totalLevels),
             10, 10, 20, WHITE);
    DrawText(TextFormat("Completed: %d/%d", gameData.levelsCompleted,
                        gameData.totalLevels),
             10, 35, 20, WHITE);
    DrawText(TextFormat("Health: %d/%d", world.player.health,
                        world.player.maxHealth),
             10, 60, 20, GREEN);
    DrawText(TextFormat("Spawners: %d", world.spawnerCount), 10, 85, 18,
             YELLOW);
    DrawText(TextFormat("Bullets: %d", world.bulletCount), 150, 85, 18, RED);
    if (currentState == STATE_PAUSED) {
      DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 180});
      DrawText("PAUSED", SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 - 100, 40,
               YELLOW);
      const char *pauseOptions[] = {"Resume", "Save Game", "Quit to Menu"};
      for (int i = 0; i < 3; i++) {
        Color c = (i == pauseSelection) ? YELLOW : WHITE;
        DrawText(pauseOptions[i], SCREEN_WIDTH / 2 - 80,
                 SCREEN_WIDTH / 2 - 20 + i * 35, 24, c);
      }
    }
  } else if (currentState == STATE_LEVEL_COMPLETE) {
    World_Draw(&world);
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 150});
    DrawText("LEVEL COMPLETE!", SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 2 - 40,
             40, GOLD);
    DrawText("Press ENTER to continue", SCREEN_WIDTH / 2 - 110,
             SCREEN_HEIGHT / 2 + 20, 20, WHITE);
  } else if (currentState == STATE_GAME_COMPLETE) {
    ClearBackground((Color){20, 20, 40, 255});
    DrawText("CONGRATULATIONS!", SCREEN_WIDTH / 2 - 160, SCREEN_HEIGHT / 2 - 80,
             40, GOLD);
    DrawText("You completed all levels!", SCREEN_WIDTH / 2 - 120,
             SCREEN_HEIGHT / 2 - 20, 20, WHITE);
    DrawText(TextFormat("%d/%d Levels Completed", gameData.levelsCompleted,
                        gameData.totalLevels),
             SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 20, 20, GREEN);
    DrawText("Press ENTER to return to menu", SCREEN_WIDTH / 2 - 140,
             SCREEN_HEIGHT / 2 + 60, 20, WHITE);
  }
  if (saveMenuState != SAVE_MENU_NONE) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});
    const char *title =
        (saveMenuState == SAVE_MENU_SAVE) ? "SAVE GAME" : "LOAD GAME";
    DrawText(title, SCREEN_WIDTH / 2 - 80, 100, 32, YELLOW);
    for (int i = 0; i < MAX_SAVE_SLOTS; i++) {
      Color c = (i == saveSlotSelection) ? YELLOW : WHITE;
      int y = 200 + i * 50;
      if (saveSlots[i].isValid) {
        DrawText(TextFormat("[%d] %s - Level %d", i + 1, saveSlots[i].saveName,
                            saveSlots[i].currentLevel + 1),
                 SCREEN_WIDTH / 2 - 150, y, 20, c);
      } else {
        DrawText(TextFormat("[%d] Empty Slot", i + 1), SCREEN_WIDTH / 2 - 150,
                 y, 20, c);
      }
    }
    DrawText("Arrow Keys + ENTER to select | ESC to cancel",
             SCREEN_WIDTH / 2 - 180, 500, 18, GRAY);
  }
  if (achievementNotifTimer > 0 && currentState != STATE_MENU) {
    const Achievement *ach = Achievement_Get(
        &gameData.achievements, gameData.achievements.lastUnlocked);
    if (ach) {
      float alpha =
          (achievementNotifTimer > 4.5f)
              ? ((ACHIEVEMENT_NOTIF_DURATION - achievementNotifTimer) / 0.5f)
          : (achievementNotifTimer < 0.5f) ? (achievementNotifTimer / 0.5f)
                                           : 1.0f;
      int notifW = 400;
      int notifH = 100;
      int notifX = SCREEN_WIDTH - notifW - 20;
      int notifY = 20;
      Color bgColor = (Color){30, 30, 50, (unsigned char)(200 * alpha)};
      Color borderColor = (Color){255, 215, 0, (unsigned char)(255 * alpha)};
      Color textColor = (Color){255, 255, 255, (unsigned char)(255 * alpha)};
      Color titleColor = (Color){255, 215, 0, (unsigned char)(255 * alpha)};
      DrawRectangle(notifX, notifY, notifW, notifH, bgColor);
      DrawRectangleLinesEx((Rectangle){notifX, notifY, notifW, notifH}, 3,
                           borderColor);
      DrawText("ACHIEVEMENT UNLOCKED!", notifX + 20, notifY + 10, 16,
               titleColor);
      DrawText(ach->name, notifX + 20, notifY + 35, 20, textColor);
      DrawText(ach->description, notifX + 20, notifY + 65, 14, textColor);
    }
  }
}
void Game_Cleanup(void) {
  if (worldLoaded) {
    Assets_StopMusic(&world.assets);
    World_Unload(&world);
  }
  if (editorLoaded) {
    Assets_StopMusic(&editor.assets);
    Level_Unload(&editor.level);
    Assets_Unload(&editor.assets);
  }
  CloseAudioDevice();
}
const AchievementSystem *Game_GetAchievementSystem(void) {
  return &gameData.achievements;
}
