#pragma once

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

extern bool FLAG_ResizeViewports;

void ImGui_PassEvent(SDL_Event* event);
void ImGui_Init(struct GlobalContainer* gbl);
void ImGui_StartFrame();
void ImGui_Render();
void ImGui_Shutdown();


#ifdef __cplusplus
}
#endif

