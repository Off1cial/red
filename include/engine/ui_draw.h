#pragma once

#include <glad/glad.h>

#define GUI_MAX_VERTICES 2048

typedef struct guivertex_s
{
  float pos[2];
  float uv[2];
  float col[4];
} guivertex_t;



typedef struct guibatch_s
{
  guivertex_t* verts;
  int vertcount;

  unsigned int* indices;
  int indcount;

} guibatch_t;

// Batch for filled, batch for outlined?


void GuiBatch_Create();
void GuiBatch_Reset();
void GUiBatch_Destroy();

void GuiBatch_AddRectFilled(float pos[2][4], float col[4]);