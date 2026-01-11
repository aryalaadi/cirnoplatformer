#include "config.h"
#include "game.h"
#include "raylib.h"
int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Cirno's Hardest Parkour");
  SetTargetFPS(60);
  SetExitKey(KEY_NULL);
  Game_Init();
  while (!WindowShouldClose()) {
    Game_Update();
    BeginDrawing();
    ClearBackground((Color){135, 206, 235, 255});
    Game_Draw();
    EndDrawing();
  }
  Game_Cleanup();
  CloseWindow();
  return 0;
}
