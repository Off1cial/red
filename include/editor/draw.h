#pragma once

// Specialised renderer for the editor that is shit and unoptimised
#include "engine/mesh.h"
#include "corebase/mathlib.h"
#include "engine/assetmanager.h"



typedef struct brush_s brush_t;


uint8_t Brush_UpdateRenderable(brush_t* brush);

void R_DrawBrush(brush_t* b);

void R_DrawBrushes(brush_t* list);

void R_DrawSurfaces();
