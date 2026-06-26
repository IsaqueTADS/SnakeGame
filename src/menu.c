#include "menu.h"
#include "audio.h"
#include "heart.h"
#include "snake.h"

#define MAX_DIFF 4

GameState state = MENU_MAIN;
int selectedDiff = 1;
int highScore = 0;

static const char *diffNames[MAX_DIFF] = {
    "F\u00e1cil", "Normal", "M\u00e9dio", "Insuport\u00e1vel"
};

static const float diffLives[MAX_DIFF] = { 5.0f, 4.0f, 3.5f, 2.0f };
static const double diffInterval[MAX_DIFF] = { 0.18, 0.13, 0.09, 0.05 };
static const Color diffColors[MAX_DIFF] = { GREEN, BLUE, ORANGE, RED };

static bool Button(Rectangle rec, const char *text, int fontSize, Color bg, Color hover, Color textCol)
{
    Vector2 m = GetMousePosition();
    bool h = CheckCollisionPointRec(m, rec);
    DrawRectangleRounded(rec, 0.15f, 8, h ? hover : bg);
    int tw = MeasureText(text, fontSize);
    DrawText(text, (int)(rec.x + rec.width / 2 - tw / 2), (int)(rec.y + rec.height / 2 - fontSize / 2), fontSize, textCol);
    if (h && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        PlaySound(GetSfxClick());
        return true;
    }
    return false;
}

void UpdateMainMenu(void)
{
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    float bw = 220, bh = 55;
    float bx = w / 2.0f - bw / 2.0f;

    if (Button((Rectangle){ bx, h / 2.0f + 30, bw, bh }, "Jogar", 28,
               (Color){ 30, 80, 200, 255 }, (Color){ 50, 120, 255, 255 }, RAYWHITE))
        state = MENU_DIFFICULTY;

    if (Button((Rectangle){ bx, h / 2.0f + 100, bw, bh }, "Sair", 28,
               (Color){ 180, 30, 30, 255 }, (Color){ 220, 50, 50, 255 }, RAYWHITE))
        CloseWindow();
}

void DrawMainMenu(void)
{
    ClearBackground((Color){ 15, 15, 25, 255 });
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    DrawText("SNAKE", (w - MeasureText("SNAKE", 80)) / 2, h / 2 - 130, 80, (Color){ 50, 255, 80, 255 });
    DrawText("GAME", (w - MeasureText("GAME", 36)) / 2, h / 2 - 50, 36, GRAY);
}

void UpdateDifficultyMenu(void)
{
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    float bw = 280, bh = 52;
    float bx = w / 2.0f - bw / 2.0f;
    float startY = h / 2.0f - 130;

    for (int i = 0; i < MAX_DIFF; i++)
    {
        float by = startY + i * (bh + 12);
        Color darker = (Color){ diffColors[i].r / 2, diffColors[i].g / 2, diffColors[i].b / 2, 255 };
        if (Button((Rectangle){ bx, by, bw, bh }, diffNames[i], 24, darker, diffColors[i], RAYWHITE))
        {
            selectedDiff = i;
            InitSnakeGame(diffInterval[i], diffLives[i]);
            state = PLAYING;
        }
    }

    if (Button((Rectangle){ 16, 16, 44, 44 }, "\xe2\x86\x90", 28,
               (Color){ 40, 40, 60, 255 }, (Color){ 60, 60, 80, 255 }, RAYWHITE))
        state = MENU_MAIN;
}

void DrawDifficultyMenu(void)
{
    ClearBackground((Color){ 15, 15, 25, 255 });
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    DrawText("Selecione a Dificuldade", (w - MeasureText("Selecione a Dificuldade", 32)) / 2, 50, 32, RAYWHITE);

    float startY = h / 2.0f - 130;
    int heartSize = 4;
    int hx = w / 2 - 80;

    for (int i = 0; i < MAX_DIFF; i++)
    {
        int hy = (int)(startY + i * (52 + 12)) + 58;
        DrawLives(diffLives[i], hx, hy, heartSize);
    }
}
