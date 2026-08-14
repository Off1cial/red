
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "engine/camera.h"

camera_t* gCamera = NULL;

static void mat4identitytest(camera_t* cam)
{
  mat4 vp, invVP, test;

Mat4Mul(cam->projection, cam->view, vp);
Mat4Inverse(vp, invVP);

Mat4Mul(vp, invVP, test);

printf("VP * invVP:\n");
for (int r = 0; r < 4; r++)
{
    printf(
        "%f %f %f %f\n",
        test[0][r],
        test[1][r],
        test[2][r],
        test[3][r]
    );
}
}

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
  VectorNormalise(camera->front); 

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


void Camera_Screenray(camera_t* cam, float x, float y, vec3_t outorigin, vec3_t outdir)
{
  float vx = cam->viewport[0];
  float vy = cam->viewport[1];
  float vw = cam->viewport[2];
  float vh = cam->viewport[3];

  // Pixel -> NDC [-1, 1], flip Y (screen Y down, NDC Y up)
  float ndc_x = ((x - vx) / vw) * 2.0f - 1.0f;
  float ndc_y = 1.0f - ((y - vy) / vh) * 2.0f;

  // Build inverse view-projection
  mat4 vp, invVP;
  Mat4Mul(cam->projection, cam->view, vp);   // check your lib's multiply order
  Mat4Inverse(vp, invVP);

  // Near and far points in clip space (z = -1 near, z = 1 far in NDC, w = 1)
  vec4_t clipNear = { ndc_x, ndc_y, -1.0f, 1.0f };
  vec4_t clipFar  = { ndc_x, ndc_y,  1.0f, 1.0f };

  vec4_t worldNear, worldFar;
  Mat4Mulv(invVP, clipNear, worldNear);
  Mat4Mulv(invVP, clipFar,  worldFar);

  // Perspective divide
  worldNear[0] /= worldNear[3];
  worldNear[1] /= worldNear[3];
  worldNear[2] /= worldNear[3];

  worldFar[0] /= worldFar[3];
  worldFar[1] /= worldFar[3];
  worldFar[2] /= worldFar[3];

  VectorCopy(worldNear, outorigin);

  vec3_t dir;
  VectorSub(worldFar, worldNear, dir);
  VectorNormalise(dir);
  VectorCopy(dir, outdir);
}



