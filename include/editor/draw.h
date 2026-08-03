#pragma once

// Specialised renderer for the editor that is shit and unoptimised
#include "engine/mesh.h"

typedef struct brushrender_t
{
  CBaseMesh* mesh;
  int old; // Recompute mesh
} brushrender_t;

typedef struct brush_s brush_t;

void R_DrawBrush(brush_t* b);
