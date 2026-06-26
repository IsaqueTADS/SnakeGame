#include "heart.h"

void DrawPixelHeart(int x, int y, int s, Color color, bool half)
{
    static const int pattern[5][5] = {
        {0, 1, 0, 1, 0},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {0, 1, 1, 1, 0},
        {0, 0, 1, 0, 0},
    };

    int cols = half ? 3 : 5;
    for (int r = 0; r < 5; r++)
        for (int c = 0; c < cols; c++)
            if (pattern[r][c])
                DrawRectangle(x + c * s, y + r * s, s, s, color);
}

void DrawLives(float lv, int x, int y, int heartSize)
{
    int full = (int)lv;
    bool half = (lv - full) >= 0.49f;
    int total = full + (half ? 1 : 0);

    for (int i = 0; i < total; i++)
    {
        bool isHalf = (half && i == full);
        DrawPixelHeart(x + i * (heartSize * 5 + 6), y, heartSize, RED, isHalf);
    }
}
