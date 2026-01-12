#ifndef VN_H
#define VN_H
#include "level.h"
#include "raylib.h"
typedef struct
{
	VNDialogue *dialogues;
	int dialogueCount;
	int currentDialogue;
	int textProgress;
	float textTimer;
	bool isComplete;
	Texture2D characterTexture;
	bool hasTexture;
} VNState;
void VN_Init(VNState *vn, Level *level);
void VN_Update(VNState *vn, float dt);
void VN_Draw(const VNState *vn);
void VN_Unload(VNState *vn);
bool VN_IsComplete(const VNState *vn);
void VN_Skip(VNState *vn);
#endif
