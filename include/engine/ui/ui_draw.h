#pragma once

#include <glad/glad.h>

#include "engine/ui/ui.h"

// Currently single colour
void UI_DrawRect(rectdef rect, rgba col);

void UI_DrawRectOutline(rectdef rect, rgba col, float thickness);

void UI_DrawTriangle(
    float v0[2], float v1[2], float v2[2],
    rgba colour
    );


