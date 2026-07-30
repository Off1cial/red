#ifndef CBASE_CAMERA_H
#define CBASE_CAMERA_H


#include "corebase/mathlib.h"

typedef union cViewport
{
  struct 
  {
    float x,y,w,h;
  };
  float xywh[4];
} cViewport; 

typedef struct camera_t
{
  vec3_t origin;
  qangle angles;
  vec3_t front;
  vec3_t right;
  vec3_t up;

  float fov;
  float far, near;
  float viewport[4];

  mat4 view;
  mat4 projection;
    
} camera_t;


camera_t* Camera_Create(vec3_t position, vec3_t direction, cViewport viewport);

void Camera_Look(camera_t* camera, float m_dx, float m_dy, float sens);

void Camera_Update(camera_t* camera);
void Camera_Destroy(camera_t* camera);


typedef camera_t camera_t;

#endif

