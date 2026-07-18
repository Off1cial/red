#ifndef CBASE_CAMERA_H
#define CBASE_CAMERA_H


#include "corebase/vectors.h"


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
  Vector origin;
  QAngle angles;
  Vector front;
  Vector right;
  Vector up;

  float fov;
  float far, near;
  float viewport[4];

  mat4 view;
  mat4 projection;
    
} camera_t;


camera_t* Camera_Create(Vector position, Vector direction, cViewport viewport);

void Camera_Look(camera_t* camera, float m_dx, float m_dy, float sens);

void Camera_Update(camera_t* camera);
void Camera_Destroy(camera_t* camera);


typedef camera_t camera_t;

#endif

