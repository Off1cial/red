#pragma once

#include <glad/glad.h>

#include "engine/ui/ui.h"

// Currently single colour
void UI_DrawRect(rectdef rect, rgba col);

void UI_DrawRectOutline(rectdef rect, rgba col, float thickness);


