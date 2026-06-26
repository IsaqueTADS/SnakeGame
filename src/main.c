#include <raylib.h>

float frequency = 440.0f;
float amplitude = 0.3f;
float fase = 0.0f;

#define MIN(a, b) ((a) < (b) ? (a) : (b))

void AudioInputCallback(void *bufferData, unsigned int frames)
{
    float *output = (float *)bufferData;
    float sampleRate = 44100.0f;
    float increment = frequency / sampleRate;
    for (unsigned int i = 0; i < frames; i++)
    {
        if (fase < 0.5)
            output[i] = amplitude;
        else
            output[i] = -amplitude;
    }
    fase += increment;
    if (fase >= 1.0f)
    {
        fase -= 1.0f;
    }
}

int main()
{

    const int screenWidth = 640;
    const int screenHeight = 320;

    InitWindow(screenWidth, screenHeight, "CHIP-8 Emulator");
    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(4096);
    AudioStream stream = LoadAudioStream(44100, 32, 1);
    SetaudioStreamCallback(stream, AudioInputCallback);
    PlayAudioStream(stream);

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

        if (IsKeyDown(KEY_UP))
            frequency += 2.0f;
        if (IsKeyDown(KEY_DOWN))
            frequency -= 2.0f;

        BeginDrawing();
        DrawText("Gerador de onda quadrada", 180, 150, 20, RAYWHITE);
        DrawText(TextFormat("Frequência: %.2f Hz", frequency), 240, 200, 20, RAYWHITE);
        DrawText("Use as setas para cima e para baixo para ajustar a frequência", 120, 350, 20, RAYWHITE);
        EndDrawing();

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
    StopAudioStream(stream);
    UnloadAudioStream(stream);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}