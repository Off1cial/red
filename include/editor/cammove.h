#pragma once

#include "engine/camera.h"


typedef struct camcmd_t
{
  float forward;
  float side;
  float up;
} camcmd_t;


extern camcmd_t gCamcmd;

void Camcmd_prepare();
void Camcmd_act(camera_t* camera, float dt);
