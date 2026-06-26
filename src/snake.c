#include "snake.h"
#include "audio.h"
#include "heart.h"
#include <stdlib.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static Vec2 snake[MAX_SNAKE];
static int snakeLen;
static Direction dir;
static Direction nextDir;
static Vec2 food;
static int score;
static float lives;
static double timer;
static double interval;
static bool dead;
static int overScore;

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

void InitSnakeGame(double moveInterval, float startLives)
{
    snakeLen = 3;
    for (int i = 0; i < snakeLen; i++)
        snake[i] = (Vec2){ GRID_W / 2 - i, GRID_H / 2 };

    dir = RIGHT;
    nextDir = RIGHT;
    score = 0;
    dead = false;
    overScore = 0;
    interval = moveInterval;
    timer = 0;
    lives = startLives;

    do
        food = (Vec2){ rand() % GRID_W, rand() % GRID_H };
    while (food.x == snake[0].x && food.y == snake[0].y);
}

void UpdateSnakeGame(void)
{
    if (dead)
        return;

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
    if (timer < interval)
        return;

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
            dead = true;
            overScore = score;
            PlaySound(GetSfxDie());
        }
        else
        {
            timer = -0.3;
        }
        return;
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
            dead = true;
            overScore = score;
            PlaySound(GetSfxDie());
        }
        else
        {
            timer = -0.3;
        }
        return;
    }

    for (int i = snakeLen - 1; i > 0; i--)
        snake[i] = snake[i - 1];
    snake[0] = head;

    if (head.x == food.x && head.y == food.y)
    {
        snakeLen++;
        snake[snakeLen - 1] = snake[snakeLen - 2];
        score++;
        PlaySound(GetSfxEat());
        if (interval > 0.04)
            interval -= 0.002;
        SpawnFood();
    }
}

void DrawSnakeGame(void)
{
    int cellSize = MIN(GetScreenWidth() / (GRID_W + 4), GetScreenHeight() / (GRID_H + 8));
    if (cellSize < 4) cellSize = 4;
    int gridW = cellSize * GRID_W;
    int gridH = cellSize * GRID_H;
    int ox = (GetScreenWidth() - gridW) / 2;
    int oy = (GetScreenHeight() - gridH) / 2 + 40;

    for (int x = 0; x <= GRID_W; x++)
        DrawLine(ox + x * cellSize, oy, ox + x * cellSize, oy + gridH, (Color){ 30, 30, 50, 255 });
    for (int y = 0; y <= GRID_H; y++)
        DrawLine(ox, oy + y * cellSize, ox + gridW, oy + y * cellSize, (Color){ 30, 30, 50, 255 });

    DrawRectangle(ox + food.x * cellSize + 1, oy + food.y * cellSize + 1, cellSize - 2, cellSize - 2, RED);

    for (int i = 0; i < snakeLen; i++)
    {
        Color c = i == 0 ? (Color){ 50, 255, 80, 255 } : (Color){ 30, 180, 50, 255 };
        DrawRectangle(ox + snake[i].x * cellSize + 1, oy + snake[i].y * cellSize + 1, cellSize - 2, cellSize - 2, c);
    }

    DrawLives(lives, 12, 8, 5);
    DrawText(TextFormat("SCORE: %d", score), 12, 42, 18, RAYWHITE);

    if (dead)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){ 0, 0, 0, 180 });
        const char *over = "GAME OVER";
        int fSize = 50;
        DrawText(over, (GetScreenWidth() - MeasureText(over, fSize)) / 2, GetScreenHeight() / 2 - 60, fSize, RED);
        DrawText(TextFormat("Score: %d", overScore), (GetScreenWidth() - MeasureText(TextFormat("Score: %d", overScore), 30)) / 2, GetScreenHeight() / 2, 30, RAYWHITE);
        const char *tip = "Press ENTER or SPACE to continue";
        fSize = 18;
        DrawText(tip, (GetScreenWidth() - MeasureText(tip, fSize)) / 2, GetScreenHeight() / 2 + 50, fSize, GRAY);
    }
}

bool IsSnakeDead(void) { return dead; }
int GetSnakeScore(void) { return score; }
