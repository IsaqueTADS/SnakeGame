#ifndef SNAKE_H
#define SNAKE_H

#include <raylib.h>
#include <stdbool.h>

#define GRID_W 40
#define GRID_H 30
#define MAX_SNAKE (GRID_W * GRID_H)

typedef enum { UP, DOWN, LEFT, RIGHT } Direction;
typedef struct { int x; int y; } Vec2;

void InitSnakeGame(double moveInterval, float startLives);
void UpdateSnakeGame(void);
void DrawSnakeGame(void);
bool IsSnakeDead(void);
int GetSnakeScore(void);

#endif
