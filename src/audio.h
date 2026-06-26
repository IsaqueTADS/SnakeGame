#ifndef AUDIO_H
#define AUDIO_H

#include <raylib.h>

void InitSounds(void);
Sound GetSfxEat(void);
Sound GetSfxDie(void);
Sound GetSfxClick(void);
void UnloadAllSounds(void);

#endif
