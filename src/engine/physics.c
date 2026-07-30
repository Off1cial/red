#include "engine/physics.h"
#include "engine/simd.h"
#include "platform/memarena.h"
#include <stdalign.h>

static inline int AlignedNew_Safe(void** dst, size_t c, size_t align)
{
  if (!dst)
    return 0;
  void* new = ALIGNED_NEW(c, align);
  if (!new)
    return 0;
  
  *dst = new;
  return 1;
}

int CBasePhysBodies_Init(CBasePhysBodies* array, size_t capacity)
{
  if (!array)
    return 0;
    

  array->origin       =  SIMDVEC3_New(capacity);
  array->velocity     =  SIMDVEC3_New(capacity);
  array->acceleration =  SIMDVEC3_New(capacity); 
  array->aabb_max     =  SIMDVEC3_New(capacity);
  array->aabb_min     =  SIMDVEC3_New(capacity);

  array->invmass = calloc(capacity, sizeof(float));
    
  array->capacity = capacity;
  array->count = 0;

  return 1;
}




void CBasePhysBodies_Free(CBasePhysBodies* array)
{
  SIMDVEC3_Free(&array->origin);
  SIMDVEC3_Free(&array->velocity);
  SIMDVEC3_Free(&array->acceleration);
  SIMDVEC3_Free(&array->aabb_min);
  SIMDVEC3_Free(&array->aabb_max);
  free(array->invmass);
  array->capacity = 0;
  array->count = 0;
}

uint8_t grow_physbodyarr(CBasePhysBodies* arr)
{
  if (!arr)
    return 0;

  size_t newcap = arr->capacity * 2;
  // Grow each simd array
  // ffs
  return 1;
}

uint32_t CBasePhysBodies_Create(
    CBasePhysBodies* arr,
    vec3_t origin,
    vec3_t velocity,
    vec3_t acceleration,
    float mass,
    vec3_t aabb_min,
    vec3_t aabb_max
    )
{
  if (!arr)
    return ((uint32_t)-1);
  
  // if (!grow_physbodyarr)
  // ....
  uint32_t i = arr->count;
  arr->origin.x[i] = origin[0];
  arr->origin.y[i] = origin[1];
  arr->origin.z[i] = origin[2];

  arr->velocity.x[i] = velocity[0];
  arr->velocity.y[i] = velocity[1];
  arr->velocity.z[i] = velocity[2];

  arr->acceleration.x[i] = acceleration[0];
  arr->acceleration.y[i] = acceleration[1];
  arr->acceleration.z[i] = acceleration[2];

  arr->aabb_min.x[i] = aabb_min[0];
  arr->aabb_min.y[i] = aabb_min[1];
  arr->aabb_min.z[i] = aabb_min[2];

  arr->aabb_max.x[i] = aabb_max[0];
  arr->aabb_max.y[i] = aabb_max[1];
  arr->aabb_max.z[i] = aabb_max[2];

  arr->invmass[i] = 1.0f / mass;

  arr->count++;
  return i;
}




void Physbody_AddForce(CBasePhysBodies* arr, uint32_t body, vec3_t force)
{
  // F = ma, a = F/m = F * ( 1.0f / m )
  if (body >= arr->count)
    return;
  
  float invmass = arr->invmass[body];
  arr->acceleration.x[body] += force[0] * invmass;
  arr->acceleration.y[body] += force[1] * invmass;
  arr->acceleration.z[body] += force[2] * invmass;
}

static void physbody_updatevelocity(CBasePhysBodies* arr, float dt)
{
  if (!arr)
    return;

  SIMDVEC3_Integrate(&arr->velocity, &arr->velocity, &arr->acceleration, dt, arr->count);
}

static void physbody_updateposition(CBasePhysBodies* arr, float dt)
{
  simdvec3_t posdelta;
  SIMDVEC3_Integrate(&arr->origin, &arr->origin, &arr->velocity, dt, arr->count);

}


void CBasePhysBodies_Update(CBasePhysBodies* arr, float dt)
{
  if (!arr)
    return;

  physbody_updatevelocity(arr, dt);
  physbody_updateposition(arr, dt);
}
