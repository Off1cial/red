#pragma once

#include <stdlib.h>
#include <cpuid.h>
// Header file to link ASM AVX code



typedef struct SIMDARR_vec3
{
  float *x;
  float *y;
  float *z;

  size_t count;
} simdvec3_t;

simdvec3_t SIMDVEC3_New(size_t count);
void           SIMDVEC3_Free(simdvec3_t* v);


// Automatically
void SIMD_Vec3Add(
    simdvec3_t* dst, 
    simdvec3_t* a,
    simdvec3_t* b,
    size_t count);

void SSE_FloatsAdd(
    float* dst,
    float* a,
    float* b,
    size_t count
    );


int SIMDVEC3_Add(simdvec3_t* out, simdvec3_t* a, simdvec3_t* b, int count);

int SIMDVEC3_Integrate(simdvec3_t* out, simdvec3_t* a, simdvec3_t* b, float m, int count);
