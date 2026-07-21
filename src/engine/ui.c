#include "engine/ui.h"

#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>
#include "engine/shader.h"

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


  // Style
  // Perhaps pull colours from a file?

  static rgb colButtonIdle = {10, 50, 180};
  static rgb colButtonHovered = {10, 120, 180};
  static rgb colButtonActive = {10, 180, 210};

  uistyle_t* style = &gUIctx->style;
  RGBCopy(style->button_idle, colButtonIdle);
  RGBCopy(style->button_hovered, colButtonHovered);
  RGBCopy(style->button_clicked, colButtonActive);

  // OpenGL

  glGenVertexArrays(1, &gUIctx->vao);
  glGenBuffers(1, &gUIctx->vbo);
  glGenBuffers(1, &gUIctx->ebo);
  
  gUIctx->shader = CBaseShader_Create(
      "../Assets/Shaders/vert_ui.vs",
      "../Assets/Shaders/frag_ui.fs");
  
  return 1;
}

void UI_FrameBegin()
{
  gUIctx->hot_id = -1;
  gUIctx->active_id = -1;

  gUIctx->vertexcount = 0;
  gUIctx->indexcount = 0;


}
