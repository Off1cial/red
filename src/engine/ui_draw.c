#include "engine/ui_draw.h"
#include <stdlib.h>

#define UI32_INVALID ((uint32_t)-1) // 0xFFFFFFFF

static uint8_t grow_vertex_array()
{
  size_t newcap = gUIctx->vertexcapacity * 2;
  size_t c = sizeof(uivertex_t) * newcap;
  uivertex_t* vertices = realloc(
      gUIctx->vertices,
      c);
  
  if (!vertices)
    return 0;
  
  gUIctx->vertices = vertices;

  return 1;
}

static uint8_t grow_index_array()
{
  size_t newcap = gUIctx->indexcapacity * 2;
  size_t c = sizeof(uint32_t) * newcap;
  uint32_t* indices = realloc(
      gUIctx->indices,
      c
      );

  if (!indices)
    return 0;

  gUIctx->indices = indices;

  return 1;
}

static uint32_t UI_PushVertex(uivertex_t v)
{
  if (!gUIctx->vertices)
    return UI32_INVALID;
  
  if (gUIctx->vertexcount >= gUIctx->vertexcapacity)
  {
    if (!grow_vertex_array())
      return UI32_INVALID;
  }

  gUIctx->vertices[gUIctx->vertexcount] = v;
  return gUIctx->vertexcount++;
}

static uint8_t UI_PushTriangle(
    uint32_t i0, 
    uint32_t i1,
    uint32_t i2
)
{
  if (i0 == UI32_INVALID ||
      i1 == UI32_INVALID ||
      i2 == UI32_INVALID)
    return 0;

  if (gUIctx->indexcount >= gUIctx->indexcapacity)
  {
    if (!grow_index_array())
      return 0;
  }

  gUIctx->indices[gUIctx->indexcount++] = i0; 
  gUIctx->indices[gUIctx->indexcount++] = i1; 
  gUIctx->indices[gUIctx->indexcount++] = i2; 
  return 1;
}


static void rect_corner(rectdef rect, int i, float out[2])
{
  switch(i)
  {
    case 0:
      out[0] = rect[RECT_X];
      out[1] = rect[RECT_Y];
      break;
    case 1:
      out[0] = rect[RECT_X];
      out[1] = rect[RECT_Y] + rect[RECT_H];
      break;
    case 2:
      out[0] = rect[RECT_X] + rect[RECT_W];
      out[1] = rect[RECT_Y] + rect[RECT_H];
      break;
    case 3:
      out[0] = rect[RECT_X] + rect[RECT_W];
      out[1] = rect[RECT_Y];
      break;
    default:
      break;
  }
}

static void rect_corner_all(rectdef rect, float out[2][4])
{
  for (int i = 0; i < 4; i++)
    rect_corner(rect, i, out[i]);
}

void UI_DrawRect(rectdef rect, rgba col)
{
  // Upload vertices to the context batch
  uivertex_t rectv[4] = {0};
  // Do UVs in the future
  rect_corner(rect, 0, rectv[0].pos);
  rect_corner(rect, 1, rectv[1].pos);
  rect_corner(rect, 2, rectv[2].pos);
  rect_corner(rect, 3, rectv[3].pos);

  uint32_t i0 = UI_PushVertex(rectv[0]);
  uint32_t i1 = UI_PushVertex(rectv[1]);
  uint32_t i2 = UI_PushVertex(rectv[2]);

  UI_PushTriangle(i0, i1, i2);
}
