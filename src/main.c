#include <raylib.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define GRID_W 40
#define GRID_H 30
#define MAX_SNAKE (GRID_W * GRID_H)
#define MAX_DIFFICULTIES 4

typedef enum { UP, DOWN, LEFT, RIGHT } Direction;
typedef enum { MENU_MAIN, MENU_DIFFICULTY, PLAYING, GAME_OVER } GameState;

typedef struct {
    int x;
    int y;
} Vec2;

typedef struct {
    const char *name;
    float lives;
    double interval;
    Color color;
} Difficulty;

Vec2 snake[MAX_SNAKE];
int snakeLen;
Direction dir;
Direction nextDir;
Vec2 food;
int score;
int highScore;
float lives;
double timer;
double interval;

GameState state;
int selectedDiff;
bool gameOver;
int overScore;

Sound sfxEat;
Sound sfxDie;
Sound sfxClick;

static Wave GenSweep(float fStart, float fEnd, float dur, unsigned int sampleRate)
{
    int count = (int)(sampleRate * dur);
    short *data = malloc(count * sizeof(short));
    for (int i = 0; i < count; i++)
    {
        float t = (float)i / sampleRate;
        float freq = fStart + (fEnd - fStart) * (t / dur);
        data[i] = (short)(sinf(2 * PI * freq * t) * 16000);
    }
    return (Wave){ .data = data, .frameCount = count, .sampleRate = sampleRate, .sampleSize = 16, .channels = 1 };
}

static void InitSounds(void)
{
    Wave w = GenSweep(500, 1200, 0.1f, 44100);
    sfxEat = LoadSoundFromWave(w);
    UnloadWave(w);

    w = GenSweep(400, 60, 0.6f, 44100);
    sfxDie = LoadSoundFromWave(w);
    UnloadWave(w);

    w = GenSweep(600, 800, 0.05f, 44100);
    sfxClick = LoadSoundFromWave(w);
    UnloadWave(w);
}

static const Difficulty difficulties[MAX_DIFFICULTIES] = {
    { "F\u00e1cil",       5.0f, 0.18, GREEN },
    { "Normal",          4.0f, 0.13, BLUE },
    { "M\u00e9dio",      3.5f, 0.09, ORANGE },
    { "Insuport\u00e1vel", 2.0f, 0.05, RED },
};

static void DrawPixelHeart(int x, int y, int s, Color color, bool half)
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

static void DrawLives(float lv, int x, int y, int heartSize)
{
    int fullHearts = (int)lv;
    bool hasHalf = (lv - fullHearts) >= 0.49f;
    int total = fullHearts + (hasHalf ? 1 : 0);

    for (int i = 0; i < total; i++)
    {
        bool half = (hasHalf && i == fullHearts);
        Color c = half ? RED : RED;
        DrawPixelHeart(x + i * (heartSize * 5 + 6), y, heartSize, c, half);
    }
}

static void InitGame(void)
{
    snakeLen = 3;
    for (int i = 0; i < snakeLen; i++)
        snake[i] = (Vec2){ GRID_W / 2 - i, GRID_H / 2 };

    dir = RIGHT;
    nextDir = RIGHT;
    score = 0;
    gameOver = false;
    overScore = 0;
    interval = difficulties[selectedDiff].interval;
    timer = 0;
    lives = difficulties[selectedDiff].lives;

    do
        food = (Vec2){ rand() % GRID_W, rand() % GRID_H };
    while (food.x == snake[0].x && food.y == snake[0].y);
}

static void SpawnFood(void)
{
    bool onSnake;
    do
    {
        onSnake = false;
        food = (Vec2){ rand() % GRID_W, rand() % GRID_H };
        for (int i = 0; i < snakeLen; i++)
        {
            if (food.x == snake[i].x && food.y == snake[i].y)
            {
                onSnake = true;
                break;
            }
        }
    } while (onSnake);
}

static bool Button(Vector2 pos, float w, float h, const char *text, int fontSize, Color bg, Color hover, Color textColor)
{
    Rectangle rec = { pos.x, pos.y, w, h };
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, rec);

    DrawRectangleRounded(rec, 0.15f, 8, hovered ? hover : bg);
    if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        PlaySound(sfxClick);
        return true;
    }

    int tw = MeasureText(text, fontSize);
    DrawText(text, (int)(pos.x + w / 2 - tw / 2), (int)(pos.y + h / 2 - fontSize / 2), fontSize, textColor);
    return false;
}

static void UpdateMainMenu(void)
{
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    float bw = 220, bh = 60;
    float bx = w / 2.0f - bw / 2.0f;
    float by = h / 2.0f + 60;

    if (Button((Vector2){ bx, by }, bw, bh, "Jogar", 30, (Color){ 30, 80, 200, 255 }, (Color){ 50, 120, 255, 255 }, RAYWHITE))
        state = MENU_DIFFICULTY;
}

static void DrawMainMenu(void)
{
    ClearBackground((Color){ 15, 15, 25, 255 });
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    DrawText("SNAKE", (w - MeasureText("SNAKE", 80)) / 2, h / 2 - 140, 80, (Color){ 50, 255, 80, 255 });
    DrawText("GAME", (w - MeasureText("GAME", 40)) / 2, h / 2 - 60, 40, GRAY);
}

static void UpdateDifficultyMenu(void)
{
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    float bw = 280, bh = 55;
    float bx = w / 2.0f - bw / 2.0f;
    float startY = h / 2.0f - 140;

    for (int i = 0; i < MAX_DIFFICULTIES; i++)
    {
        float by = startY + i * (bh + 15);
        Color darker = (Color){
            (unsigned char)(difficulties[i].color.r / 2),
            (unsigned char)(difficulties[i].color.g / 2),
            (unsigned char)(difficulties[i].color.b / 2),
            255
        };
        if (Button((Vector2){ bx, by }, bw, bh, difficulties[i].name, 26, darker, difficulties[i].color, RAYWHITE))
        {
            selectedDiff = i;
            InitGame();
            state = PLAYING;
        }
    }
}

static void DrawDifficultyMenu(void)
{
    ClearBackground((Color){ 15, 15, 25, 255 });
    int w = GetScreenWidth();
    DrawText("Selecione a Dificuldade", (w - MeasureText("Selecione a Dificuldade", 36)) / 2, 60, 36, RAYWHITE);

    int h = GetScreenHeight();
    float startY = h / 2.0f - 140;
    int heartSize = 4;
    int hx = w / 2 - 80, hy;

    for (int i = 0; i < MAX_DIFFICULTIES; i++)
    {
        hy = (int)(startY + i * (55 + 15)) + 55 + 8;
        DrawLives(difficulties[i].lives, hx, hy, heartSize);
    }
}

static void UpdateGame(void)
{
    if (gameOver)
    {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
        {
            if (score > highScore)
                highScore = score;
            state = MENU_MAIN;
        }
        return;
    }

    if (IsKeyPressed(KEY_UP) && dir != DOWN)
        nextDir = UP;
    if (IsKeyPressed(KEY_DOWN) && dir != UP)
        nextDir = DOWN;
    if (IsKeyPressed(KEY_LEFT) && dir != RIGHT)
        nextDir = LEFT;
    if (IsKeyPressed(KEY_RIGHT) && dir != LEFT)
        nextDir = RIGHT;

    if (IsKeyPressed(KEY_W) && dir != DOWN)
        nextDir = UP;
    if (IsKeyPressed(KEY_S) && dir != UP)
        nextDir = DOWN;
    if (IsKeyPressed(KEY_A) && dir != RIGHT)
        nextDir = LEFT;
    if (IsKeyPressed(KEY_D) && dir != LEFT)
        nextDir = RIGHT;

    timer += GetFrameTime();
    if (timer >= interval)
    {
        timer -= interval;
        dir = nextDir;

        Vec2 head = snake[0];
        switch (dir)
        {
            case UP:    head.y--; break;
            case DOWN:  head.y++; break;
            case LEFT:  head.x--; break;
            case RIGHT: head.x++; break;
        }

        if (head.x < 0 || head.x >= GRID_W || head.y < 0 || head.y >= GRID_H)
        {
            lives -= 1.0f;
            if (lives <= 0)
            {
                gameOver = true;
                overScore = score;
                if (score > highScore)
                    highScore = score;
                PlaySound(sfxDie);
                return;
            }
            else
            {
                timer = -0.3;
                return;
            }
        }

        bool selfHit = false;
        for (int i = 0; i < snakeLen; i++)
        {
            if (head.x == snake[i].x && head.y == snake[i].y)
            {
                selfHit = true;
                break;
            }
        }

        if (selfHit)
        {
            lives -= 0.5f;
            if (lives <= 0)
            {
                gameOver = true;
                overScore = score;
                if (score > highScore)
                    highScore = score;
                PlaySound(sfxDie);
                return;
            }
            else
            {
                timer = -0.3;
                return;
            }
        }

        for (int i = snakeLen - 1; i > 0; i--)
            snake[i] = snake[i - 1];
        snake[0] = head;

        if (head.x == food.x && head.y == food.y)
        {
            snakeLen++;
            snake[snakeLen - 1] = snake[snakeLen - 2];
            score++;
            PlaySound(sfxEat);
            if (interval > 0.04)
                interval -= 0.002f;
            SpawnFood();
        }
    }
}

static void DrawGame(void)
{
    BeginDrawing();
    ClearBackground((Color){ 15, 15, 25, 255 });

    int cellSize = MIN(GetScreenWidth() / (GRID_W + 4), GetScreenHeight() / (GRID_H + 8));
    if (cellSize < 4) cellSize = 4;
    int gridW = cellSize * GRID_W;
    int gridH = cellSize * GRID_H;
    int offsetX = (GetScreenWidth() - gridW) / 2;
    int offsetY = (GetScreenHeight() - gridH) / 2 + 40;

    for (int x = 0; x <= GRID_W; x++)
        DrawLine(offsetX + x * cellSize, offsetY, offsetX + x * cellSize, offsetY + gridH, (Color){ 30, 30, 50, 255 });
    for (int y = 0; y <= GRID_H; y++)
        DrawLine(offsetX, offsetY + y * cellSize, offsetX + gridW, offsetY + y * cellSize, (Color){ 30, 30, 50, 255 });

    DrawRectangle(offsetX + food.x * cellSize + 1, offsetY + food.y * cellSize + 1, cellSize - 2, cellSize - 2, RED);

    for (int i = 0; i < snakeLen; i++)
    {
        Color c = i == 0 ? (Color){ 50, 255, 80, 255 } : (Color){ 30, 180, 50, 255 };
        DrawRectangle(offsetX + snake[i].x * cellSize + 1, offsetY + snake[i].y * cellSize + 1, cellSize - 2, cellSize - 2, c);
    }

    DrawLives(lives, 12, 8, 5);

    DrawText(TextFormat("SCORE: %d", score), 12, 42, 18, RAYWHITE);
    DrawText(TextFormat("BEST: %d", highScore > score ? highScore : score), 12, 64, 14, GRAY);

    if (gameOver)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){ 0, 0, 0, 180 });
        const char *over = "GAME OVER";
        int fSize = 50;
        DrawText(over, (GetScreenWidth() - MeasureText(over, fSize)) / 2, GetScreenHeight() / 2 - 60, fSize, RED);
        DrawText(TextFormat("Score: %d", overScore), (GetScreenWidth() - MeasureText(TextFormat("Score: %d", overScore), 30)) / 2, GetScreenHeight() / 2, 30, RAYWHITE);
        const char *restart = "Press ENTER or SPACE to continue";
        fSize = 18;
        DrawText(restart, (GetScreenWidth() - MeasureText(restart, fSize)) / 2, GetScreenHeight() / 2 + 50, fSize, GRAY);
    }

    EndDrawing();
}

static void DrawMenu(void)
{
    BeginDrawing();
    if (state == MENU_MAIN)
        DrawMainMenu();
    else if (state == MENU_DIFFICULTY)
        DrawDifficultyMenu();
    EndDrawing();
}

int main(void)
{
    srand((unsigned int)time(NULL));

    InitWindow(800, 600, "Snake Game");
    InitAudioDevice();
    InitSounds();
    SetTargetFPS(60);

    state = MENU_MAIN;
    selectedDiff = 1;
    highScore = 0;

    while (!WindowShouldClose())
    {
        if (state == MENU_MAIN)
        {
            UpdateMainMenu();
            DrawMenu();
        }
        else if (state == MENU_DIFFICULTY)
        {
            UpdateDifficultyMenu();
            DrawMenu();
        }
        else if (state == PLAYING || state == GAME_OVER)
        {
            UpdateGame();
            DrawGame();
        }
    }

    UnloadSound(sfxEat);
    UnloadSound(sfxDie);
    UnloadSound(sfxClick);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
