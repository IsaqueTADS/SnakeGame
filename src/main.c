#include <raylib.h>

int main() {
    InitWindow(800, 600, "CHIP-8 Emulator");

    while (!WindowShouldClose()) {
        BeginDrawing();

        DrawRectangle(0, 0, 800, 600, (Color){0, 156, 59, 255});

        DrawPoly((Vector2){400, 300}, 4, 230, 45, (Color){255, 217, 0, 255});

        DrawCircle(400, 300, 130, (Color){0, 39, 118, 255});

        EndDrawing();
    }

    CloseWindow();
    return 0;
}