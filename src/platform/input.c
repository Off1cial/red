#include "platform/input.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

pltInput* gPltInput = NULL;

pltInput* PlatformInput_Create()
{
  pltInput* input = malloc(sizeof(pltInput));
  if (!input)
  {
    printf("[PLATFORM]: Failed to create input system\n");
    return NULL;
  }
  memset(input, 0, sizeof(pltInput));
  return input;
}


void PlatformInput_Destroy(pltInput* input)
{
  assert(input);
  memset(input, 0, sizeof(pltInput));
  free(input);
}



void PlatformInput_Poll(SDL_Window* window, pltInput* input, int* quit)
{
  input->eventWindowResized = 0;
  


  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
      case SDL_EVENT_QUIT:
        *quit = 1; // Exits the main loop
        break;

      case SDL_EVENT_WINDOW_RESIZED:
        input->eventWindowResized = 1;
        input->eventWindowNewWidth = event.window.data1;
        input->eventWindowNewHeight = event.window.data2;
        break;
    }
  }

  const bool* keys = SDL_GetKeyboardState(NULL);

  SDL_MouseButtonFlags mdata;

  if (input->mouselocked)
  {
    mdata = SDL_GetRelativeMouseState(&input->mxrel, &input->myrel);
  }else
  {
    mdata = SDL_GetMouseState(&input->mx, &input->my);
  }
  
  memcpy(input->mPrevious, input->mCurrent, sizeof(input->mCurrent));
  input->mCurrent[0] = (mdata & SDL_BUTTON_LMASK) != 0;
  input->mCurrent[1] = (mdata & SDL_BUTTON_RMASK) != 0;
  input->mCurrent[2] = (mdata & SDL_BUTTON_MMASK) != 0;
  input->mCurrent[3] = (mdata & SDL_BUTTON_X1MASK) != 0;
  input->mCurrent[4] = (mdata & SDL_BUTTON_X2MASK) != 0;

  memcpy(input->kPrevious, input->kCurrent, sizeof(input->kCurrent));
  memcpy(input->kCurrent, keys, SDL_SCANCODE_COUNT);

  for (int i = 0; i < SDL_SCANCODE_COUNT; i++)
  {
    input->pressed[i]  = input->kCurrent[i] && !input->kPrevious[i];
    input->released[i] = !input->kCurrent[i] && input->kPrevious[i]; 
  }

  if (input->pressed[SDL_SCANCODE_ESCAPE])
    pltInput_ToggleMouseLock(input, window);

}

