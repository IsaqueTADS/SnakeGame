#ifndef MENU_H
#define MENU_H

#include <raylib.h>

typedef enum { MENU_MAIN, MENU_DIFFICULTY, PLAYING, GAME_OVER } GameState;

extern GameState state;
extern int selectedDiff;
extern int highScore;

void UpdateMainMenu(void);
void UpdateDifficultyMenu(void);
void DrawMainMenu(void);
void DrawDifficultyMenu(void);

#endif
