#include "corebase/mathlib.h"
#include <math.h>

/*

double VectorLength(vec3_t v)
{
  return sqrt(VectorLength2(v));
}

double VectorNormalise(vec3_t v, vec3_t out)
{
  double mag = VectorLength(v);
  if (mag)
  {
    double imag = 1.0f / mag;
    out[0] = v[0] * imag;
    out[1] = v[1] * imag;
    out[2] = v[2] * imag;
  }
  return mag;
}

void VectorCrossProduct (vec3_t v1, vec3_t v2, vec3_t cross)
	cross[0] = v1[1]*v2[2] - v1[2]*v2[1];
	cross[1] = v1[2]*v2[0] - v1[0]*v2[2];
	cross[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

*/
