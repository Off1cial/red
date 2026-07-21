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
    


  // Extra
  bool eventWindowResized;
  int eventWindowNewWidth;
  int eventWindowNewHeight;

} pltInput;

extern pltInput* gPltInput;

pltInput*   PlatformInput_Create();
void        PlatformInput_Destroy(pltInput* input);


void        PlatformInput_Poll(pltInput* input, int* quit);


#define pltInput_MouseClick(button) \
  gPltInput->mCurrent[button] && !gPltInput->mPrevious[button]

#define pltInput_MouseRelease(button) \
  !gPltInput->mCurrent[button] && gPltInput->mPrevious[button]

#define pltInput_MouseDown(button) \
  gPltInput->mCurrent[button]
