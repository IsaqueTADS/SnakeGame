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
    int x = 32, y = 16;

    while (!WindowShouldClose())
    {
        if (IsWindowResized)
        {
            int scale = MIN(GetScreenWidth() / 64, GetScreenHeight() / 32);
            int offsetX = (GetScreenWidth() - 64 * scale) / 2;
            int offsetY = (GetScreenHeight() - 32 * scale) / 2;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        if (IsKeyDown(KEY_W) && y > 0)
        {
            tela[y][x] = false;
            y--;
        }
        if (IsKeyDown(KEY_S) && y < 31)
        {
            tela[y][x] = false;
            y++;
        }
        if (IsKeyDown(KEY_A) && x > 0)
        {
            tela[y][x] = false;
            x--;
        }
        if (IsKeyDown(KEY_D) && x < 63)
        {
            tela[y][x] = false;
            x++;
        }

        tela[y][x] = true;

        for (int y = 0; y < 32; y++)
        {
            for (int x = 0; x < 64; x++)
            {
                if (tela[y][x])
                {
                    DrawRectangle(offsetX + x * scale, offsetY + y * scale, scale, 2 * scale, RAYWHITE);
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}