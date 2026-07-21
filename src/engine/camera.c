
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "engine/camera.h"



camera_t* Camera_Create(Vector position, Vector direction, cViewport viewport)
{
  camera_t* cam = malloc(sizeof(camera_t)); 
  if (!cam) return NULL;
  memset(cam, 0, sizeof(camera_t));

  VectorCopy(position, &cam->origin);
  VectorCopy(direction, &cam->front);

  memcpy(cam->viewport, &viewport, sizeof(float) * 4);

  cam->angles = VectorAngles(direction);
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
  camera->front = QAngleVector(camera->angles);
  camera->front = VectorNormalise(camera->front); 
  
  camera->right = VectorCrossNormalise(camera->front, VECTOR_AXIS_Y);
  camera->up    = VectorCrossNormalise(camera->right, camera->front);

  camera->view = Mat4LookAt(
                  camera->origin,
                  VectorAdd(camera->origin, camera->front),
                  camera->up
                 );

  camera->projection = Mat4Perspective(
                        camera->fov,
                        camera->viewport[2] / camera->viewport[3],
                        camera->near,
                        camera->far
                       );
}

void Camera_Look(camera_t* camera, float m_dx, float m_dy, float sens)
{
  camera->angles.x -= m_dy * sens;
  camera->angles.y += m_dx * sens;
  
  if (camera->angles.x > 89.0f)
    camera->angles.x = 89.0f;
  if (camera->angles.x < - 89.0f)
    camera->angles.y = -89.0f;

}
