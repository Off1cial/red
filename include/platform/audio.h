#pragma once

#include <SDL3/SDL.h>


typedef struct PLT_AUDIO_DEVICE
{
  SDL_AudioDeviceID id;
  SDL_AudioStream* stream;

  int frequency;
  int channels;

  float volume; // master
} pltAudioDevice;

