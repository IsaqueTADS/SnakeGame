#ifndef HEART_H
#define HEART_H

#include <raylib.h>
#include <stdbool.h>

void DrawPixelHeart(int x, int y, int s, Color color, bool half);
void DrawLives(float lives, int x, int y, int heartSize);

#endif
