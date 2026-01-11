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

#include "vn.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
void VN_Init(VNState *vn, Level *level) {
  vn->dialogues = level->dialogues;
  vn->dialogueCount = level->dialogueCount;
  vn->currentDialogue = 0;
  vn->textProgress = 0;
  vn->textTimer = 0;
  vn->isComplete = false;
  vn->hasTexture = false;
  if (vn->dialogueCount > 0 && vn->dialogues[0].characterSprite[0] != '\0') {
    if (FileExists(vn->dialogues[0].characterSprite)) {
      vn->characterTexture = LoadTexture(vn->dialogues[0].characterSprite);
      vn->hasTexture = true;
    }
  }
}
void VN_Update(VNState *vn, float dt) {
  if (vn->isComplete)
    return;
  VNDialogue *current = &vn->dialogues[vn->currentDialogue];
  int textLength = strlen(current->text);
  vn->textTimer += dt;
  if (vn->textTimer >= 0.03f) {
    vn->textTimer = 0;
    if (vn->textProgress < textLength) {
      vn->textProgress++;
    }
  }
  if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    if (vn->textProgress < textLength) {
      vn->textProgress = textLength;
    } else {
      vn->currentDialogue++;
      if (vn->currentDialogue >= vn->dialogueCount) {
        vn->isComplete = true;
      } else {
        vn->textProgress = 0;
        vn->textTimer = 0;
        VNDialogue *next = &vn->dialogues[vn->currentDialogue];
        if (vn->hasTexture &&
            strcmp(current->characterSprite, next->characterSprite) != 0) {
          UnloadTexture(vn->characterTexture);
          vn->hasTexture = false;
        }
        if (!vn->hasTexture && next->characterSprite[0] != '\0') {
          if (FileExists(next->characterSprite)) {
            vn->characterTexture = LoadTexture(next->characterSprite);
            vn->hasTexture = true;
          }
        }
      }
    }
  }
}
void VN_Draw(const VNState *vn) {
  if (vn->isComplete)
    return;
  VNDialogue *current = &vn->dialogues[vn->currentDialogue];
  Color bgColors[] = {(Color){20, 20, 40, 255}, (Color){40, 20, 20, 255},
                      (Color){20, 40, 20, 255}, (Color){40, 40, 40, 255},
                      BLACK};
  Color bgColor = bgColors[current->bgColor % 5];
  ClearBackground(bgColor);
  if (vn->hasTexture) {
    float scale = (float)SCREEN_HEIGHT / vn->characterTexture.height * 0.8f;
    DrawTextureEx(
        vn->characterTexture,
        (Vector2){SCREEN_WIDTH / 2 - vn->characterTexture.width * scale / 2,
                  50},
        0, scale, WHITE);
  } else {
    DrawRectangle(SCREEN_WIDTH / 2 - 100, 100, 200, 200,
                  (Color){100, 100, 150, 200});
    DrawText("CHARACTER", SCREEN_WIDTH / 2 - 80, 190, 20, WHITE);
  }
  Rectangle dialogueBox = {50, SCREEN_HEIGHT - 200, SCREEN_WIDTH - 100, 150};
  DrawRectangleRec(dialogueBox, (Color){10, 10, 30, 230});
  DrawRectangleLinesEx(dialogueBox, 3, GOLD);
  DrawText(current->characterName, 60, SCREEN_HEIGHT - 190, 24, YELLOW);
  char displayText[300];
  strncpy(displayText, current->text, vn->textProgress);
  displayText[vn->textProgress] = '\0';
  int maxWidth = SCREEN_WIDTH - 120;
  DrawText(displayText, 70, SCREEN_HEIGHT - 150, 20, WHITE);
  if (vn->textProgress >= strlen(current->text)) {
    if (((int)(GetTime() * 3)) % 2 == 0) {
      DrawText(">", SCREEN_WIDTH - 80, SCREEN_HEIGHT - 70, 30, YELLOW);
    }
  }
  DrawText("SPACE/CLICK to continue", SCREEN_WIDTH / 2 - 100,
           SCREEN_HEIGHT - 40, 16, GRAY);
}
void VN_Unload(VNState *vn) {
  if (vn->hasTexture) {
    UnloadTexture(vn->characterTexture);
    vn->hasTexture = false;
  }
}
bool VN_IsComplete(const VNState *vn) { return vn->isComplete; }
void VN_Skip(VNState *vn) { vn->isComplete = true; }
