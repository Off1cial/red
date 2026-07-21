#include "engine/ui.h"

#include <stdlib.h>
#include <string.h>

uicontext_t* gUIctx = NULL;


uint8_t UI_Init()
{
  gUIctx = malloc(sizeof(uicontext_t));
  memset(gUIctx, 0, sizeof(uicontext_t));

  gUIctx->vertexcapacity = 128;
  gUIctx->indexcapacity = 128;

  size_t cv = sizeof(uivertex_t) * gUIctx->vertexcapacity;
  size_t ci = sizeof(uint32_t) * gUIctx->indexcapacity;

  gUIctx->vertices = malloc(cv);
  gUIctx->indices  = malloc(ci);
      
  if (!gUIctx->vertices || !gUIctx->indices)
    return 0;


  return 1;
}

void UI_FrameBegin()
{
  gUIctx->vertexcount = 0;
  gUIctx->indexcapacity = 0;

  // Take input from SDL
}
