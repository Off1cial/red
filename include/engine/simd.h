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
} simdarr_vec3_t;

simdarr_vec3_t SIMDVEC3_New(size_t count);
void           SIMDVEC3_Free(simdarr_vec3_t* v);


// Automatically
void SIMD_Vec3Add(
    simdarr_vec3_t* dst, 
    simdarr_vec3_t* a,
    simdarr_vec3_t* b,
    size_t count);

void SSE_FloatsAdd(
    float* dst,
    float* a,
    float* b,
    size_t count
    );


// Returns the number of remaining floats to be worked on sequentially
int AVX_Add(float* dst, float* a, float* b, int count);
int AVX_IntegrateFMA3(float* dst, float* a, float* b, float m, int count);
int AVX_Integrate(float* dst, float* a, float* b, float m, int count);


