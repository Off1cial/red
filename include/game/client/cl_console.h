#pragma once

#include "engine/ui/ui.h"
#define CONSOLE_TEXTLENGTH 32768

typedef uint16_t conchar_t;

#define CONCHAR(ch, col) \
    ((((uint16_t)(col)) << 8) | (uint8_t)(ch))

#define CONCHAR_CHAR(x)   ((char)((x) & 0xFF))
#define CONCHAR_COLOUR(x) ((uint8_t)((x) >> 8))

typedef struct console_s
{
  // Ring buffer approach
  conchar_t text[CONSOLE_TEXTLENGTH]; // NOTE 1
  int currentline; // Line number for the next print
  int linecount;
  int linewidth;
  
  rectdef rect;

} console_t;

// NOTE 1
// 
// Why is the text buffer 'uint16_t'
// Because the 0-7 bits can be the character and the rest can be colour
//

extern console_t gConsole;

void Console_Draw();
