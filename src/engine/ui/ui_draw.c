#include "engine/ui/ui_draw.h"
#include <stdlib.h>
#include "platform/common.h"
#include "platform/window.h"
#include "engine/shader.h"

#define UI32_INVALID ((uint32_t)-1) // 0xFFFFFFFF

static void UI_ScreenToNDC(float x, float y, float out[2])
{
    out[0] = (x / gPltWindow->winw) * 2.0f - 1.0f;
    out[1] = 1.0f - (y / gPltWindow->winh) * 2.0f;
}

static inline void uivertex_setndcpos(uivertex_t* v, float pos[2])
{
  UI_ScreenToNDC(pos[0], pos[1], v->pos);
}

static inline void uivertex_setcolour(uivertex_t* v, rgba colour)
{
  v->colour[0] = colour[0] / 255; v->colour[1] = colour[1] / 255;
  v->colour[2] = colour[2] / 255; v->colour[3] = colour[3] / 255;
}

static void uidebug_printvert(float pos[2])
{
  printf("UIvert (%0.2f, %0.2f)\n", pos[0], pos[1]);
}

static void uidebug_printcolour(float col[4])
{
  printf("Col: %0.2f, %0.2f, %0.2f, %0.2f\n", col[0], col[1], col[2], col[3]);
}



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
  gUIctx->vertexcapacity = newcap;

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
  gUIctx->indexcapacity = newcap;

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


  if (gUIctx->indexcount + 3 >= gUIctx->indexcapacity)
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

static void rect_corner_all(rectdef rect, float out[4][2])
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
  
  for (int i = 0; i < 4; i++)
  {
    rectv[i].colour[0] = col[0] / 255.0f;
    rectv[i].colour[1] = col[1] / 255.0f;
    rectv[i].colour[2] = col[2] / 255.0f;
    rectv[i].colour[3] = col[3] / 255.0f;

    UI_ScreenToNDC(rectv[i].pos[0], rectv[i].pos[1], rectv[i].pos);
    //uidebug_printvert(rectv[i].pos);
    //uidebug_printcolour(rectv[i].colour);

  }

  // Since it uses openGL, i can have gradients
  //rectv[3].colour[0] = 1.0f;
  //rectv[2].colour[1] = 1.0f;
  uint32_t i0 = UI_PushVertex(rectv[0]);
  uint32_t i1 = UI_PushVertex(rectv[1]);
  uint32_t i2 = UI_PushVertex(rectv[2]);
  uint32_t i3 = UI_PushVertex(rectv[3]);

  UI_PushTriangle(i0, i1, i2);
  UI_PushTriangle(i0, i2, i3);
}

void UI_DrawRectOutline(rectdef rect, rgba col, float thickness)
{
    rectdef top = {
        rect[RECT_X],
        rect[RECT_Y],
        rect[RECT_W],
        thickness
    };

    rectdef bottom = {
        rect[RECT_X],
        rect[RECT_Y] + rect[RECT_H] - thickness,
        rect[RECT_W],
        thickness
    };

    rectdef left = {
        rect[RECT_X],
        rect[RECT_Y],
        thickness,
        rect[RECT_H]
    };

    rectdef right = {
        rect[RECT_X] + rect[RECT_W] - thickness,
        rect[RECT_Y],
        thickness,
        rect[RECT_H]
    };

    UI_DrawRect(top, col);
    UI_DrawRect(bottom, col);
    UI_DrawRect(left, col);
    UI_DrawRect(right, col);
}

void UI_DrawTriangle(float v0[2], float v1[2], float v2[2], rgba colour)
{
  uivertex_t verts[3];
  memset(verts, 0, sizeof(uivertex_t)* 3);

  uivertex_setndcpos(&verts[0], v0);
  uivertex_setndcpos(&verts[1], v1);
  uivertex_setndcpos(&verts[2], v2);


  uivertex_setcolour(&verts[0], colour);
  uivertex_setcolour(&verts[1], colour);
  uivertex_setcolour(&verts[2], colour);

  GLuint i0 = UI_PushVertex(verts[0]);
  GLuint i1 = UI_PushVertex(verts[1]);
  GLuint i2 = UI_PushVertex(verts[2]);

  UI_PushTriangle(i0, i1, i2);
}

void UI_DrawBatch()
{
  glDisable(GL_DEPTH_TEST);   // UI is 2D — ordering, not depth, decides visibility
  glDepthMask(GL_FALSE);      // don't let UI write into the depth buffer either

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // needed if you ever use alpha < 255



  // Bind shader
  CBaseShader_Use(gUIctx->shader);

  // Upload data
  glBindVertexArray(gUIctx->vao);
  // vbo
  glBindBuffer(GL_ARRAY_BUFFER, gUIctx->vbo);
  glBufferData(
      GL_ARRAY_BUFFER,
      gUIctx->vertexcount * sizeof(uivertex_t),
      gUIctx->vertices,
      GL_DYNAMIC_DRAW
      );
  // ebo
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gUIctx->ebo);
  glBufferData(
      GL_ELEMENT_ARRAY_BUFFER,
      gUIctx->indexcount * sizeof(GLuint),
      gUIctx->indices,
      GL_DYNAMIC_DRAW
      );
  
  // Vertex data
  
  // Position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
      0, 2, 
      GL_FLOAT, GL_FALSE, 
      sizeof(uivertex_t), (void*)OFFSETOF(uivertex_t, pos));
  // UV
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
      1, 2, 
      GL_FLOAT, GL_FALSE, 
      sizeof(uivertex_t), (void*)OFFSETOF(uivertex_t, uv));
  // colour
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(
      2, 4, 
      GL_FLOAT, GL_FALSE, 
      sizeof(uivertex_t), (void*)OFFSETOF(uivertex_t, colour));
  
  glDrawElements(
      GL_TRIANGLES, 
      (GLsizei)gUIctx->indexcount,
      GL_UNSIGNED_INT,
      0);

  glBindVertexArray(0);

  glDepthMask(GL_TRUE);       // restore state for whatever draws next (next frame's 3D pass)
  glEnable(GL_DEPTH_TEST);

}
