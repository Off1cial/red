#pragma once

#include <stdint.h>
#include "corebase/mathlib.h"
#include "editor/draw.h"




u8 Brush_AddToList(brush_t* b, brush_t** list);
u8 Brush_RemoveFromList(brush_t* b, brush_t** list);

// Creates a 6-faced brush with the specified bounds
brush_t* Brush_Create(vec3_t mins, vec3_t maxs);
void Brush_Delete(brush_t* b);

