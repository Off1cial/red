#pragma once


#include <SDL3/SDL.h>
#include <glad/glad.h>


typedef struct PLT_WINDOW
{
  SDL_Window* window;
  SDL_GLContext glContext;
  int winw, winh;
} pltWindow;

pltWindow*  PlatformWindow_Create(int width, int height, const char* title);
void        PlatformWindow_Destroy(pltWindow* win);


void        PlatformWindow_Resize(pltWindow* window, int width, int height);
