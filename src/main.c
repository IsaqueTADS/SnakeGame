#include <raylib.h>
#include <stdlib.h>
#include <time.h>

#include "menu.h"
#include "snake.h"
#include "audio.h"

int main(void)
{
    srand((unsigned int)time(NULL));

    InitWindow(800, 600, "Snake Game");
    InitAudioDevice();
    InitSounds();
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (state == MENU_MAIN)
        {
            UpdateMainMenu();
            DrawMainMenu();
        }
        else if (state == MENU_DIFFICULTY)
        {
            UpdateDifficultyMenu();
            DrawDifficultyMenu();
        }
        else if (state == PLAYING || state == GAME_OVER)
        {
            UpdateSnakeGame();

            BeginDrawing();
            ClearBackground((Color){ 15, 15, 25, 255 });
            DrawSnakeGame();
            DrawText(TextFormat("BEST: %d", highScore > GetSnakeScore() ? highScore : GetSnakeScore()), 12, 64, 14, GRAY);
            EndDrawing();

            if (IsSnakeDead() && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)))
            {
                if (GetSnakeScore() > highScore)
                    highScore = GetSnakeScore();
                state = MENU_MAIN;
            }
        }
    }

    UnloadAllSounds();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
