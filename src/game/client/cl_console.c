#include "game/client/cl_console.h"

#include "engine/ui/ui_draw.h"


console_t gConsole = {0};
vec4_t gConsoleCOL_BG = {
  255 * 0.3,
  255 * 0.3,
  255 * 0.3,
  255
};

static inline short NewChar(char col, char character)
{
  return (col << 8) | character;
}

void Console_Goto(int linenum)
{
  gConsole.currentline = linenum;
  if (linenum >= gConsole.linecount)
    gConsole.currentline = linenum -1;
}

void Console_Clear()
{
  gConsole.linecount = 0;
  gConsole.currentline = 0;
}

void Console_PutLine(const char* line)
{
  return;
}

void Console_Draw()
{
  // Draw window rect
  //UI_DrawRect(gConsole.rect, 0, 0, 1, 1, gConsoleCOL_BG, -1);

  if (UI_Begin("Console", gConsole.rect, 0))
  {
    // Do shit
  }
  UI_End();
}
