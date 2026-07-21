#include "corebase/time.h"
#include <unistd.h>
#include <SDL3/SDL.h>

static Uint64 start_time = 0.0f;

void pltTime_Init( void )
{
  start_time = SDL_GetPerformanceCounter();
}

double pltTime_Time( void )
{
  return (double)(SDL_GetPerformanceCounter() - start_time) / (double)SDL_GetPerformanceFrequency();
}
