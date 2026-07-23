
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "engine/camera.h"



camera_t* Camera_Create(vec3_t position, vec3_t direction, cViewport viewport)
{
  camera_t* cam = malloc(sizeof(camera_t)); 
  if (!cam) return NULL;
  memset(cam, 0, sizeof(camera_t));

  VectorCopy(position, cam->origin);
  VectorCopy(direction, cam->front);

  memcpy(cam->viewport, &viewport, sizeof(float) * 4);

  VectorAngles(direction, cam->angles);
  cam->fov = RAD(90.0f);
  cam->near = 0.1f;
  cam->far = 1000.0f;
  Camera_Update(cam);
  return cam;
}

void Camera_Destroy(camera_t* camera)
{
  assert(camera);
  memset(camera, 0, sizeof(camera_t));
  free(camera);
}

void Camera_Update(camera_t* camera)
{
  QAngleVector(camera->angles, camera->front);
  VectorNormalise(camera->front, camera->front); 

  VectorCrossNorm(camera->front, VEC_AXIS_Y, camera->right);
  VectorCrossNorm(camera->right, camera->front, camera->up);
  
  vec3_t centre;
  VectorAdd(camera->origin, camera->front, centre);

  Mat4LookAt(              
    camera->view,
    camera->origin,
    centre,
    camera->up
  );

  Mat4Perspective(
      camera->projection,
      camera->fov,
      camera->viewport[2] / camera->viewport[3],
      camera->near,
      camera->far
  );
}

void Camera_Look(camera_t* camera, float m_dx, float m_dy, float sens)
{
  camera->angles[PITCH] -= m_dy * sens;
  camera->angles[YAW] += m_dx * sens;
  
  if (camera->angles[PITCH] > 89.0f)
    camera->angles[PITCH] = 89.0f;
  if (camera->angles[PITCH] < - 89.0f)
    camera->angles[PITCH] = -89.0f;

}
