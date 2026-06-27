#include "music.h"
#include <raylib.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define SR 44100
#define PI 3.14159265358979323846f

static Music bgm;
static unsigned char *buf;
static int bufSize;
static bool active;

static void w16(unsigned char *d, int o, short v)
{
    d[o] = (unsigned char)(v & 0xFF);
    d[o + 1] = (unsigned char)((v >> 8) & 0xFF);
}

static void w32(unsigned char *d, int o, int v)
{
    d[o] = (unsigned char)(v & 0xFF);
    d[o + 1] = (unsigned char)((v >> 8) & 0xFF);
    d[o + 2] = (unsigned char)((v >> 16) & 0xFF);
    d[o + 3] = (unsigned char)((v >> 24) & 0xFF);
}

static unsigned char *gen(const float *fr, const float *dr, int n, int *sz, float vol)
{
    int total = 0, i;
    for (i = 0; i < n; i++)
        total += (int)(dr[i] * SR);

    short *s = (short *)malloc(total * sizeof(short));
    int idx = 0;
    for (i = 0; i < n; i++)
    {
        int cnt = (int)(dr[i] * SR);
        float dur = dr[i];
        for (int j = 0; j < cnt; j++)
        {
            float t = (float)j / SR;
            float v = 0;
            if (fr[i] > 0)
            {
                float atk = 0.005f, rel = 0.015f;
                float env = 1.0f;
                if (t < atk) env = t / atk;
                if (t > dur - rel) env = (dur - t) / rel;
                if (env < 0) env = 0;
                v = sinf(2 * PI * fr[i] * t) * vol * env;
            }
            s[idx++] = (short)(v * 32767);
        }
    }

    int dataSz = total * 2;
    int fileSz = 44 + dataSz;
    unsigned char *w = (unsigned char *)malloc(fileSz);

    memcpy(w, "RIFF", 4);
    w32(w, 4, fileSz - 8);
    memcpy(w + 8, "WAVE", 4);
    memcpy(w + 12, "fmt ", 4);
    w32(w, 16, 16);
    w16(w, 20, 1);
    w16(w, 22, 1);
    w32(w, 24, SR);
    w32(w, 28, SR * 2);
    w16(w, 32, 2);
    w16(w, 34, 16);
    memcpy(w + 36, "data", 4);
    w32(w, 40, dataSz);
    memcpy(w + 44, s, dataSz);

    free(s);
    *sz = fileSz;
    return w;
}

static unsigned char *gen2(const float *f1, const float *d1, int n1,
                           const float *f2, const float *d2, int n2,
                           int *sz, float v1, float v2)
{
    float t1 = 0, t2 = 0;
    for (int i = 0; i < n1; i++) t1 += d1[i];
    for (int i = 0; i < n2; i++) t2 += d2[i];
    float totalDur = (t1 > t2) ? t1 : t2;
    int total = (int)(totalDur * SR);
    short *s = (short *)calloc(total, sizeof(short));

    int idx = 0;
    for (int i = 0; i < n1; i++)
    {
        int cnt = (int)(d1[i] * SR);
        float dur = d1[i];
        for (int j = 0; j < cnt && idx + j < total; j++)
        {
            float t = (float)j / SR;
            float vt = 0;
            if (f1[i] > 0)
            {
                float atk = 0.005f, rel = 0.015f;
                float env = 1.0f;
                if (t < atk) env = t / atk;
                if (t > dur - rel) env = (dur - t) / rel;
                if (env < 0) env = 0;
                vt = sinf(2 * PI * f1[i] * t) * v1 * env;
            }
            s[idx + j] += (short)(vt * 32767);
        }
        idx += cnt;
    }

    idx = 0;
    for (int i = 0; i < n2; i++)
    {
        int cnt = (int)(d2[i] * SR);
        float dur = d2[i];
        for (int j = 0; j < cnt && idx + j < total; j++)
        {
            float t = (float)j / SR;
            float vt = 0;
            if (f2[i] > 0)
            {
                float atk = 0.005f, rel = 0.015f;
                float env = 1.0f;
                if (t < atk) env = t / atk;
                if (t > dur - rel) env = (dur - t) / rel;
                if (env < 0) env = 0;
                vt = sinf(2 * PI * f2[i] * t) * v2 * env;
            }
            s[idx + j] += (short)(vt * 32767);
        }
        idx += cnt;
    }

    int mx = 0;
    for (int i = 0; i < total; i++)
    {
        int a = s[i] < 0 ? -s[i] : s[i];
        if (a > mx) mx = a;
    }
    float norm = (mx > 32767) ? 32767.0f / mx : 1.0f;

    int dataSz = total * 2;
    int fileSz = 44 + dataSz;
    unsigned char *w = (unsigned char *)malloc(fileSz);

    memcpy(w, "RIFF", 4);
    w32(w, 4, fileSz - 8);
    memcpy(w + 8, "WAVE", 4);
    memcpy(w + 12, "fmt ", 4);
    w32(w, 16, 16);
    w16(w, 20, 1);
    w16(w, 22, 1);
    w32(w, 24, SR);
    w32(w, 28, SR * 2);
    w16(w, 32, 2);
    w16(w, 34, 16);
    memcpy(w + 36, "data", 4);
    w32(w, 40, dataSz);

    short *out = (short *)(w + 44);
    for (int i = 0; i < total; i++)
        out[i] = (short)(s[i] * norm);

    free(s);
    *sz = fileSz;
    return w;
}

void InitBgMusic(void)
{
    bgm = (Music){0};
    buf = NULL;
    bufSize = 0;
    active = false;
}

void StopBgMusic(void)
{
    if (active)
    {
        StopMusicStream(bgm);
        UnloadMusicStream(bgm);
        active = false;
    }
    if (buf)
    {
        free(buf);
        buf = NULL;
        bufSize = 0;
    }
}

void PlayBgMusic(int difficulty)
{
    StopBgMusic();

    float f1[16], d1[16], f2[16], d2[16];
    int n1, n2;

    switch (difficulty)
    {
        case 0:
        {
            float ff[] = {261.63f, 329.63f, 392.00f, 329.63f,
                          261.63f, 196.00f, 261.63f, 0};
            float dd[] = {1.0f, 1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, 1.0f, 1.0f};
            memcpy(f1, ff, sizeof(ff)); memcpy(d1, dd, sizeof(dd));
            n1 = 8; n2 = 0;
            buf = gen(f1, d1, n1, &bufSize, 0.35f);
            break;
        }
        case 1:
        {
            float ff[] = {261.63f, 293.66f, 329.63f, 349.23f,
                          392.00f, 349.23f, 329.63f, 293.66f};
            float dd[] = {0.5f, 0.5f, 0.5f, 0.5f,
                          0.5f, 0.5f, 0.5f, 0.5f};
            memcpy(f1, ff, sizeof(ff)); memcpy(d1, dd, sizeof(dd));
            n1 = 8; n2 = 0;
            buf = gen(f1, d1, n1, &bufSize, 0.35f);
            break;
        }
        case 2:
        {
            float ff1[] = {261.63f, 293.66f, 329.63f, 349.23f,
                           392.00f, 440.00f, 493.88f, 523.25f};
            float dd1[] = {0.3f, 0.3f, 0.3f, 0.3f,
                           0.3f, 0.3f, 0.3f, 0.3f};
            float ff2[] = {329.63f, 349.23f, 392.00f, 440.00f,
                           493.88f, 523.25f, 587.33f, 659.25f};
            float dd2[] = {0.3f, 0.3f, 0.3f, 0.3f,
                           0.3f, 0.3f, 0.3f, 0.3f};
            memcpy(f1, ff1, sizeof(ff1)); memcpy(d1, dd1, sizeof(dd1));
            memcpy(f2, ff2, sizeof(ff2)); memcpy(d2, dd2, sizeof(dd2));
            n1 = 8; n2 = 8;
            buf = gen2(f1, d1, n1, f2, d2, n2, &bufSize, 0.25f, 0.2f);
            break;
        }
        case 3:
        {
            float ff1[] = {440.00f, 523.25f, 659.25f, 783.99f,
                           659.25f, 523.25f, 440.00f, 349.23f};
            float dd1[] = {0.17f, 0.17f, 0.17f, 0.17f,
                           0.17f, 0.17f, 0.17f, 0.17f};
            float ff2[] = {220.00f, 261.63f, 329.63f, 392.00f,
                           329.63f, 261.63f, 220.00f, 174.61f};
            float dd2[] = {0.17f, 0.17f, 0.17f, 0.17f,
                           0.17f, 0.17f, 0.17f, 0.17f};
            memcpy(f1, ff1, sizeof(ff1)); memcpy(d1, dd1, sizeof(dd1));
            memcpy(f2, ff2, sizeof(ff2)); memcpy(d2, dd2, sizeof(dd2));
            n1 = 8; n2 = 8;
            buf = gen2(f1, d1, n1, f2, d2, n2, &bufSize, 0.3f, 0.2f);
            break;
        }
        default:
            return;
    }

    if (buf && bufSize > 0)
    {
        bgm = LoadMusicStreamFromMemory(".wav", buf, bufSize);
        bgm.looping = true;
        PlayMusicStream(bgm);
        active = true;
    }
}

void UpdateBgMusic(void)
{
    if (active)
        UpdateMusicStream(bgm);
}
