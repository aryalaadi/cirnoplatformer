#ifndef MENU_H
#define MENU_H
#include "config.h"
void Menu_Init(void);
void Menu_Update(void);
void Menu_Draw(void);
Settings *Menu_GetSettings(void);
void Menu_SaveSettings(void);
void Menu_LoadSettings(void);
void Menu_ApplyResolution(void);
#endif
