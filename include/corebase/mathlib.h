#ifndef NEW_VECTORS_H
#define NEW_VECTORS_H

typedef float vec_t;

typedef vec_t vec3_t[3];
typedef vec_t vec2_t[2];

#define Vector(x, y, z) ( {(x), (y), (z)})
#define VectorSub(c, a, b) ( c[0]=a[0]-b[0];c[1]=a[1]-b[1];c[2]=a[2]-b[2]; )
#define VectorAdd(c, a, b) ( c[0]=a[0]+b[0];c[1]=a[1]+b[1];c[2]=a[2]+b[2]; )
#define VectorCopy(a, b)  ( b[0]=a[0];b[1]=a[1];b[2]=a[2]; )


#define DotProduct(a, b)  ( a[0]*b[0]+a[1]*b[1]+a[2]*b[2] )


#endif