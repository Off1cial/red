#ifndef NEW_VECTORS_H
#define NEW_VECTORS_H

#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef float vec_t;

typedef vec_t vec4_t[4];
typedef vec_t vec3_t[3];
typedef vec_t vec2_t[2];

extern vec3_t VEC_AXIS_X;
extern vec3_t VEC_AXIS_Y;
extern vec3_t VEC_AXIS_Z;

extern vec3_t VEC_AXIS_X_NEG;
extern vec3_t VEC_AXIS_Y_NEG;
extern vec3_t VEC_AXIS_Z_NEG;

extern vec3_t VEC_ZERO;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define RAD(x) ((x) * (M_PI / 180.0f))
#define DEG(x) ((x) * (180.0f / M_PI))

#define EPSILON 1e-6f

// Angle indexes

#define PITCH 0
#define YAW 1
#define ROLL 2

typedef vec3_t qangle;

// Derive QAngle from vector

static inline void VectorAngles(const vec3_t v, qangle q)
{
  float pitch = atan2f(
      v[1],
      sqrtf(v[0] * v[0] + v[2] * v[2]));

  float yaw = atan2f(v[2], v[0]);

  q[PITCH] = DEG(pitch);
  q[YAW] = DEG(yaw);
  q[ROLL] = 0.0f;
}

// Derive vector from angles

static inline void QAngleVector(const qangle q, vec3_t out)
{
  float pitch = RAD(q[PITCH]);
  float yaw = RAD(q[YAW]);

  out[0] = cosf(yaw) * cosf(pitch);
  out[1] = sinf(pitch);
  out[2] = sinf(yaw) * cosf(pitch);
}

// Basic vectors

static inline void Vector(vec3_t v, float x, float y, float z)
{
  v[0] = x;
  v[1] = y;
  v[2] = z;
}

static inline void VectorCopy(const vec3_t v, vec3_t out)
{
  out[0] = v[0];
  out[1] = v[1];
  out[2] = v[2];
}

static inline void VectorAdd(
    const vec3_t a,
    const vec3_t b,
    vec3_t out)
{
  out[0] = a[0] + b[0];
  out[1] = a[1] + b[1];
  out[2] = a[2] + b[2];
}

static inline void VectorSub(
    const vec3_t a,
    const vec3_t b,
    vec3_t out)
{
  out[0] = a[0] - b[0];
  out[1] = a[1] - b[1];
  out[2] = a[2] - b[2];
}

static inline float DotProduct(
    const vec3_t a,
    const vec3_t b)
{
  return a[0] * b[0] +
         a[1] * b[1] +
         a[2] * b[2];
}

static inline float VectorLength2(const vec3_t v)
{
  return v[0] * v[0] +
         v[1] * v[1] +
         v[2] * v[2];
}

static inline float VectorLength(const vec3_t v)
{
  return sqrtf(VectorLength2(v));
}

static inline float VectorNormalise(
    const vec3_t v,
    vec3_t out)
{
  float mag = VectorLength(v);

  if (mag <= EPSILON)
  {
    Vector(out, 0.0f, 0.0f, 0.0f);
    return 0.0f;
  }

  float inv = 1.0f / mag;

  out[0] = v[0] * inv;
  out[1] = v[1] * inv;
  out[2] = v[2] * inv;

  return mag;
}

static inline void VectorCross(
    const vec3_t v1,
    const vec3_t v2,
    vec3_t cross)
{
  cross[0] = v1[1] * v2[2] - v1[2] * v2[1];
  cross[1] = v1[2] * v2[0] - v1[0] * v2[2];
  cross[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

static inline void VectorCrossNorm(
    const vec3_t v1,
    const vec3_t v2,
    vec3_t cross)
{
  VectorCross(v1, v2, cross);
  VectorNormalise(cross, cross);
}

static inline void VectorScale(vec3_t v, float scale)
{
  v[0] *= scale;
  v[1] *= scale;
  v[2] *= scale;
}

// 2D vectors

static inline void Vector2Add(
    const vec2_t a,
    const vec2_t b,
    vec2_t out)
{
  out[0] = a[0] + b[0];
  out[1] = a[1] + b[1];
}

static inline void Vector2Sub(
    const vec2_t a,
    const vec2_t b,
    vec2_t out)
{
  out[0] = a[0] - b[0];
  out[1] = a[1] - b[1];
}

static inline void Vector2Copy(
    const vec2_t v,
    vec2_t dst)
{
  dst[0] = v[0];
  dst[1] = v[1];
}

// Matrices

typedef vec_t mat4[4][4];
typedef vec_t mat3[3][3];

extern mat4 MAT4_IDENTITY;

static inline void Mat4Identity(mat4 m)
{
  memset(m, 0, sizeof(mat4));

  m[0][0] = 1.0f;
  m[1][1] = 1.0f;
  m[2][2] = 1.0f;
  m[3][3] = 1.0f;
}

static inline void Mat4Copy(
    const mat4 m,
    mat4 dest)
{
  memcpy(dest, m, sizeof(mat4));
}

static inline void Mat4Mul(
    const mat4 a,
    const mat4 b,
    mat4 dest)
{
  mat4 result;

  for (int col = 0; col < 4; col++)
  {
    for (int row = 0; row < 4; row++)
    {
      result[col][row] =
          a[0][row] * b[col][0] +
          a[1][row] * b[col][1] +
          a[2][row] * b[col][2] +
          a[3][row] * b[col][3];
    }
  }

  Mat4Copy(result, dest);
}

static inline void Mat4LookAt(
    mat4 view,
    const vec3_t eye,
    const vec3_t center,
    const vec3_t up)
{
  vec3_t back;
  vec3_t right;
  vec3_t real_up;

  // Camera backward direction
  VectorSub(eye, center, back);
  VectorNormalise(back, back);

  // Camera right direction
  VectorCross(up, back, right);
  VectorNormalise(right, right);

  // Corrected up direction
  VectorCross(back, right, real_up);
  VectorNormalise(real_up, real_up);

  Mat4Identity(view);

  view[0][0] = right[0];
  view[0][1] = real_up[0];
  view[0][2] = back[0];

  view[1][0] = right[1];
  view[1][1] = real_up[1];
  view[1][2] = back[1];

  view[2][0] = right[2];
  view[2][1] = real_up[2];
  view[2][2] = back[2];

  view[3][0] = -DotProduct(right, eye);
  view[3][1] = -DotProduct(real_up, eye);
  view[3][2] = -DotProduct(back, eye);
}

static inline void Mat4Perspective(
    mat4 out,
    float fov,
    float aspect,
    float znear,
    float zfar)
{
  float f = 1.0f / tanf(fov * 0.5f);

  memset(out, 0, sizeof(mat4));

  out[0][0] = f / aspect;
  out[1][1] = f;

  out[2][2] =
      (zfar + znear) /
      (znear - zfar);

  out[2][3] = -1.0f;

  out[3][2] =
      (2.0f * zfar * znear) /
      (znear - zfar);
}

// Planes and rays

typedef struct ray_t
{
  vec3_t origin;
  vec3_t dir;

} ray_t;

typedef struct plane_t
{
  vec3_t normal;
  vec_t d;

} plane_t;

plane_t PlaneReverse(plane_t plane);

#endif