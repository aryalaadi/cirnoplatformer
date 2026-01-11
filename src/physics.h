#ifndef PHYSICS_H
#define PHYSICS_H
#include "level.h"
#include "player.h"
void Physics_ApplyGravity(Player *p, float dt);
void Physics_MoveX(Player *p, const Level *lvl, float dt);
void Physics_MoveY(Player *p, const Level *lvl, float dt);
#endif
