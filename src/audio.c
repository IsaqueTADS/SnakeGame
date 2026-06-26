#include "audio.h"
#include <math.h>
#include <stdlib.h>

#define PI 3.14159265358979323846

static Sound sfxEat;
static Sound sfxDie;
static Sound sfxClick;

static Wave GenSweep(float fStart, float fEnd, float dur, unsigned int sampleRate)
{
    int count = (int)(sampleRate * dur);
    short *data = malloc(count * sizeof(short));
    if (!data)
        return (Wave){ 0 };

    for (int i = 0; i < count; i++)
    {
        float t = (float)i / sampleRate;
        float freq = fStart + (fEnd - fStart) * (t / dur);
        data[i] = (short)(sinf(2 * PI * freq * t) * 16000);
    }
    return (Wave){ .data = data, .frameCount = (unsigned int)count, .sampleRate = sampleRate, .sampleSize = 16, .channels = 1 };
}

void InitSounds(void)
{
    Wave w;

    w = GenSweep(500, 1200, 0.1f, 44100);
    sfxEat = LoadSoundFromWave(w);
    UnloadWave(w);

    w = GenSweep(400, 60, 0.6f, 44100);
    sfxDie = LoadSoundFromWave(w);
    UnloadWave(w);

    w = GenSweep(600, 800, 0.05f, 44100);
    sfxClick = LoadSoundFromWave(w);
    UnloadWave(w);
}

Sound GetSfxEat(void)     { return sfxEat; }
Sound GetSfxDie(void)     { return sfxDie; }
Sound GetSfxClick(void)   { return sfxClick; }

void UnloadAllSounds(void)
{
    UnloadSound(sfxEat);
    UnloadSound(sfxDie);
    UnloadSound(sfxClick);
}
