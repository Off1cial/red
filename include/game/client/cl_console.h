#pragma once

#include "engine/ui/ui.h"
#define CONSOLE_TEXTLENGTH 32768


typedef struct console_s
{
  // Ring buffer approach
  short text[CONSOLE_TEXTLENGTH]; // NOTE 1
  int currentline; // Line number for the next print
  int linecount;
  
  rectdef rect;

} console_t;

// NOTE 1
// 
// Why is the text buffer 'short'
// Because the 0-7 bits can be the character and the rest can be colour
//

extern console_t gConsole;
console_t gConsole;
