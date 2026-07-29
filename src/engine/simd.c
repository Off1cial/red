#include "engine/simd.h"
#include <string.h>
#include <stdlib.h>
#include "platform/memarena.h"

simdarr_vec3_t SIMDVEC3_New(size_t count)
{
  simdarr_vec3_t new = {0};
  new.count = count;


  new.x = calloc(count, sizeof(float));
  new.y = calloc(count, sizeof(float));
  new.z = calloc(count, sizeof(float));

  return new;

}



void SIMDVEC3_Free(simdarr_vec3_t* v)
{
  if (!v)
    return;

  v->count = 0;
  if (v->x) free(v->x);
  if (v->y) free(v->y);
  if (v->z) free(v->z);
}
