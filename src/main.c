#include <raylib.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define GRID_W 40
#define GRID_H 30
#define MAX_SNAKE (GRID_W * GRID_H)

typedef enum { UP, DOWN, LEFT, RIGHT } Direction;

typedef struct {
    int x;
    int y;
} Vec2;

Vec2 snake[MAX_SNAKE];
int snakeLen;
Direction dir;
Direction nextDir;
Vec2 food;
int score;
int highScore;
bool gameOver;
double timer;
double interval;

Sound sfxEat;
Sound sfxDie;

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
    interval = 0.13;
    timer = 0;

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

static void UpdateGame(void)
{
    if (gameOver)
    {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
        {
            if (score > highScore)
                highScore = score;
            InitGame();
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
            gameOver = true;
            PlaySound(sfxDie);
            return;
        }

        for (int i = 0; i < snakeLen; i++)
        {
            if (head.x == snake[i].x && head.y == snake[i].y)
            {
                gameOver = true;
                PlaySound(sfxDie);
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
            if (interval > 0.05)
                interval -= 0.002f;
            SpawnFood();
        }
    }
}

static void DrawGame(void)
{
    BeginDrawing();
    ClearBackground((Color){ 15, 15, 25, 255 });

    int cellSize = MIN(GetScreenWidth() / (GRID_W + 4), GetScreenHeight() / (GRID_H + 6));
    if (cellSize < 4)
        cellSize = 4;
    int gridW = cellSize * GRID_W;
    int gridH = cellSize * GRID_H;
    int offsetX = (GetScreenWidth() - gridW) / 2;
    int offsetY = (GetScreenHeight() - gridH) / 2 + 20;

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

    DrawText(TextFormat("SCORE: %d", score), 10, 10, 20, RAYWHITE);
    DrawText(TextFormat("BEST: %d", highScore > score ? highScore : score), 10, 34, 16, GRAY);

    if (gameOver)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){ 0, 0, 0, 180 });
        const char *over = "GAME OVER";
        int fSize = 50;
        DrawText(over, (GetScreenWidth() - MeasureText(over, fSize)) / 2, GetScreenHeight() / 2 - 50, fSize, RED);
        const char *restart = "Press ENTER or SPACE to restart";
        fSize = 20;
        DrawText(restart, (GetScreenWidth() - MeasureText(restart, fSize)) / 2, GetScreenHeight() / 2 + 10, fSize, GRAY);
    }

    EndDrawing();
}

int main(void)
{
    srand((unsigned int)time(NULL));

    InitWindow(800, 600, "Snake Game");
    InitAudioDevice();
    InitSounds();
    SetTargetFPS(60);

    InitGame();

    while (!WindowShouldClose())
    {
        UpdateGame();
        DrawGame();
    }

    UnloadSound(sfxEat);
    UnloadSound(sfxDie);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
