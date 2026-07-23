#pragma once

#include <glad/glad.h>

#include "engine/ui/ui.h"


#define UI_RECTCORNER_TL 0
#define UI_RECTCORNER_BL 1
#define UI_RECTCORNER_BR 2
#define UI_RECTCORNER_TR 3

// Currently single colour
void UI_DrawRect(rectdef rect, float u1, float v1, float u2, float v2, rgba col, GLint texid);

void UI_DrawRectOutline(rectdef rect, rgba col, float thickness);

void UI_DrawTriangle(
    float v0[2], float v1[2], float v2[2],
    rgba colour
    );


