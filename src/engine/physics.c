#include "engine/physics.h"
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
    
  size_t cvec = sizeof(float) * 3 * capacity;

  if (!AlignedNew_Safe((void**)&array->origin, cvec, alignof(float)))
    return 0;

  if (!AlignedNew_Safe((void**)&array->velocity, cvec, alignof(float)))
    return 0;

  if (!AlignedNew_Safe((void**)array->acceleration, cvec, alignof(float)))
    return 0;

  if (!AlignedNew_Safe((void**)&array->aabb_mins, cvec, alignof(float)))
    return 0;

  if (!AlignedNew_Safe((void**)&array->aabb_maxs, cvec, alignof(float)))
    return 0;

  if (!AlignedNew_Safe((void**)&array->invmass, sizeof(float) * 3, alignof(float)))
    return 0;



  array->capacity = capacity;
  array->count = 0;

  return 1;
}

void CBasePhysBodies_Free(CBasePhysBodies* array)
{
  ALIGNED_FREE(array->origin);
  ALIGNED_FREE(array->velocity);
  ALIGNED_FREE(array->acceleration);
  ALIGNED_FREE(array->aabb_mins);
  ALIGNED_FREE(array->aabb_maxs);
  ALIGNED_FREE(array->invmass);
  array->capacity = 0;
  array->count = 0;
}
