#include "platform/window.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


pltWindow* PlatformWindow_Create(
    int width, 
    int height, 
    const char* title)
{
  int initres = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO); 
  if (!initres)
  {
    printf("[PLATFORM]: Failed to start SDL\n");
    return NULL;
  }

  pltWindow* win = malloc(sizeof(pltWindow));
  if (!win)
  {
    printf("[PLATFORM]: Failed to allocate memory for window\n");
    return NULL;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  
  win->window = SDL_CreateWindow(
      title, 
      width, 
      height,
      SDL_WINDOW_OPENGL
      );
  
  if (!win->window)
  {
    printf("[PLATFORM]: Failed to create SDL_Window\n");
    return NULL;
  }

  win->winw = width;
  win->winh = height;


  win->glContext = SDL_GL_CreateContext(win->window);
  if (!win->glContext || 
      !gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
  {
    printf("[PLATFORM]: Failed to create OpenGL context\n");
    return NULL;
  }

  glEnable(GL_DEPTH_TEST);
  return win;
}


void PlatformWindow_Destroy(pltWindow* win)
{
  SDL_GL_DestroyContext(win->glContext);
  SDL_DestroyWindow(win->window);
  memset(win,0, sizeof(pltWindow));
  free(win);
}



void PlatformWindow_Resize(pltWindow* window, int width, int height)
{
  assert(window && window->window);
  window->winw = width;
  window->winh = height;
  glViewport(0, 0, width, height);
}
