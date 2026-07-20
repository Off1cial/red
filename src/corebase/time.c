#include "corebase/time.h"
#include <unistd.h>
#include <SDL3/SDL.h>

double pltTime_Time( void )
{
    return (double)SDL_GetPerformanceCounter() /
           (double)SDL_GetPerformanceFrequency();
}
