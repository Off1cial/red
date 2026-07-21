#ifndef NEW_VECTORS_H
#define NEW_VECTORS_H

typedef float vec_t;

typedef vec_t vec3_t[3];
typedef vec_t vec2_t[2];

/*

#define Vector(x, y, z) ( {(x), (y), (z)})
#define VectorZero ( Vector(0,0,0) )
#define VectorSub(a, b, out) ( out[0]=a[0]-b[0];out[1]=a[1]-b[1];out[2]=a[2]-b[2] )
#define VectorAdd(a, b, out) ( out[0]=a[0]+b[0];out[1]=a[1]+b[1];out[2]=a[2]+b[2] )
#define VectorCopy(a, b)  ( b[0]=a[0];b[1]=a[1];b[2]=a[2] )
#define VectorNegate(a, b) ( b[0]=-a[0];b[1]=-a[1];b[2]=-a[2] )

#define VectorLength2(v) (v[0]*v[0] + v[1]*v[1] + v[2]*v[2] )


#define DotProduct(a, b)  ( a[0]*b[0]+a[1]*b[1]+a[2]*b[2] )

double VectorLength(vec3_t v);
double _VectorNormalise(vec3_t v, vec3_t out);

void VectorCrossProduct(vec3_t v1, vec3_t v2, vec3_t cross); 

*/


#endif
