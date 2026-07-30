#pragma once

#include <SDL3/SDL.h>

typedef struct PLT_INPUT
{

  // Keyboard
  
  bool kCurrent[SDL_SCANCODE_COUNT];
  bool kPrevious[SDL_SCANCODE_COUNT];

  bool pressed[SDL_SCANCODE_COUNT];
  bool released[SDL_SCANCODE_COUNT];

  // Mouse
  bool mCurrent[5];
  bool mPrevious[5];

  float mx, my; // Cursor position relative to top-left origin
  float mxrel, myrel; // Cursor movement
  bool mouselocked;
    


  // Extra
  bool eventWindowResized;
  int eventWindowNewWidth;
  int eventWindowNewHeight;

} pltInput;

extern pltInput* gPltInput;

pltInput*   PlatformInput_Create();
void        PlatformInput_Destroy(pltInput* input);


void        PlatformInput_Poll(SDL_Window* window, pltInput* input, int* quit);


static inline uint8_t pltInput_MouseClick(int button)
{
  return gPltInput->mCurrent[button] && !gPltInput->mPrevious[button];
}

static inline uint8_t pltInput_MouseRelease(int button)
{
  return (!gPltInput->mCurrent[button]) && gPltInput->mPrevious[button];
}

static inline uint8_t pltInput_MouseDown(int button)
{
  return gPltInput->mCurrent[button];
}


static inline void pltInput_ToggleMouseLock(pltInput* input, SDL_Window* window)
{
  input->mouselocked = !input->mouselocked;
  SDL_SetWindowRelativeMouseMode(window, input->mouselocked);
}
