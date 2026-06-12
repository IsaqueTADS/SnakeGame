#include <raylib.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

int main()
{
    InitWindow(640, 320, "CHIP-8 Emulator");
    SetTargetFPS(60);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    int scale = MIN(GetScreenWidth() / 64, GetScreenHeight() / 32);
    int offsetX = (GetScreenWidth() - 64 * scale) / 2;
    int offsetY = (GetScreenHeight() - 32 * scale) / 2;
    bool tela[32][64] = {0};

    while (!WindowShouldClose())
    {
        if (IsWindowResized)
        {
            int scale = MIN(GetScreenWidth() / 64, GetScreenHeight() / 32);
            int offsetX = (GetScreenWidth() - 64 * scale) / 2;
            int offsetY = (GetScreenHeight() - 32 * scale) / 2;
        }
        for (int y = 0; y < 32; y++)
        {
            for (int x = 0; x < 64; x++)
            {
                tela[y][x] = GetRandomValue(0, 1);
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        for (int y = 0; y < 32; y++)
        {
            for (int x = 0; x < 64; x++)
            {
                if (tela[y][x])
                {
                    DrawRectangle(offsetX + x * scale, offsetY + y * scale, scale, scale, RAYWHITE);
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}