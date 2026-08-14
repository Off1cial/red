#include "corebase/mathlib.h"


vec3_t VEC_AXIS_X = {1, 0, 0}; 
vec3_t VEC_AXIS_Y = {0, 1, 0};
vec3_t VEC_AXIS_Z = {0, 0, 1};


vec3_t VEC_AXIS_X_NEG = {-1, 0, 0};
vec3_t VEC_AXIS_Y_NEG = {0, -1, 0};
vec3_t VEC_AXIS_Z_NEG = {0, 0, -1};

vec3_t VEC_ZERO = {0,0,0};

mat4 MAT4_IDENTITY;


bool Mat4Inverse(const mat4 in, mat4 out)
{
    float m[16];
    // flatten: mat4 is column-major (m[c][r]), so this matches
    // MESA's m[c*4+r] flat convention exactly, no reinterpretation
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            m[c*4 + r] = in[c][r];

    float inv[16], det;

    inv[0] = m[5]*m[10]*m[15] - m[5]*m[11]*m[14] - m[9]*m[6]*m[15]
           + m[9]*m[7]*m[14]  + m[13]*m[6]*m[11] - m[13]*m[7]*m[10];
    inv[4] = -m[4]*m[10]*m[15] + m[4]*m[11]*m[14] + m[8]*m[6]*m[15]
           - m[8]*m[7]*m[14]  - m[12]*m[6]*m[11] + m[12]*m[7]*m[10];
    inv[8] = m[4]*m[9]*m[15]  - m[4]*m[11]*m[13] - m[8]*m[5]*m[15]
           + m[8]*m[7]*m[13]  + m[12]*m[5]*m[11] - m[12]*m[7]*m[9];
    inv[12] = -m[4]*m[9]*m[14] + m[4]*m[10]*m[13] + m[8]*m[5]*m[14]
            - m[8]*m[6]*m[13] - m[12]*m[5]*m[10] + m[12]*m[6]*m[9];

    inv[1] = -m[1]*m[10]*m[15] + m[1]*m[11]*m[14] + m[9]*m[2]*m[15]
           - m[9]*m[3]*m[14]  - m[13]*m[2]*m[11] + m[13]*m[3]*m[10];
    inv[5] = m[0]*m[10]*m[15]  - m[0]*m[11]*m[14] - m[8]*m[2]*m[15]
           + m[8]*m[3]*m[14]  + m[12]*m[2]*m[11] - m[12]*m[3]*m[10];
    inv[9] = -m[0]*m[9]*m[15]  + m[0]*m[11]*m[13] + m[8]*m[1]*m[15]
           - m[8]*m[3]*m[13]  - m[12]*m[1]*m[11] + m[12]*m[3]*m[9];
    inv[13] = m[0]*m[9]*m[14]  - m[0]*m[10]*m[13] - m[8]*m[1]*m[14]
            + m[8]*m[2]*m[13] + m[12]*m[1]*m[10] - m[12]*m[2]*m[9];

    inv[2] = m[1]*m[6]*m[15]   - m[1]*m[7]*m[14]  - m[5]*m[2]*m[15]
           + m[5]*m[3]*m[14]  + m[13]*m[2]*m[7]  - m[13]*m[3]*m[6];
    inv[6] = -m[0]*m[6]*m[15]  + m[0]*m[7]*m[14]  + m[4]*m[2]*m[15]
           - m[4]*m[3]*m[14]  - m[12]*m[2]*m[7]  + m[12]*m[3]*m[6];
    inv[10] = m[0]*m[5]*m[15]  - m[0]*m[7]*m[13]  - m[4]*m[1]*m[15]
            + m[4]*m[3]*m[13] + m[12]*m[1]*m[7]  - m[12]*m[3]*m[5];
    inv[14] = -m[0]*m[5]*m[14] + m[0]*m[6]*m[13]  + m[4]*m[1]*m[14]
            - m[4]*m[2]*m[13] - m[12]*m[1]*m[6]  + m[12]*m[2]*m[5];

    inv[3] = -m[1]*m[6]*m[11]  + m[1]*m[7]*m[10]  + m[5]*m[2]*m[11]
           - m[5]*m[3]*m[10]  - m[9]*m[2]*m[7]   + m[9]*m[3]*m[6];
    inv[7] = m[0]*m[6]*m[11]   - m[0]*m[7]*m[10]  - m[4]*m[2]*m[11]
           + m[4]*m[3]*m[10]  + m[8]*m[2]*m[7]   - m[8]*m[3]*m[6];
    inv[11] = -m[0]*m[5]*m[11] + m[0]*m[7]*m[9]   + m[4]*m[1]*m[11]
            - m[4]*m[3]*m[9]  - m[8]*m[1]*m[7]   + m[8]*m[3]*m[5];
    inv[15] = m[0]*m[5]*m[10]  - m[0]*m[6]*m[9]   - m[4]*m[1]*m[10]
            + m[4]*m[2]*m[9]  + m[8]*m[1]*m[6]   - m[8]*m[2]*m[5];

    det = m[0]*inv[0] + m[1]*inv[4] + m[2]*inv[8] + m[3]*inv[12];
    if (det == 0.0f)
        return false;

    det = 1.0f / det;
    for (int i = 0; i < 16; i++)
        inv[i] *= det;

    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            out[c][r] = inv[c*4 + r];

    return true;
}

void Mat4Mulv(mat4 m, vec4_t v, vec4_t out){
  #define M(c , r) (m[c][r])

  for (int r = 0; r < 4; r++){
    out[r] =  
      M(0, r) * v[0] +
      M(1, r) * v[1] +
      M(2, r) * v[2] + 
      M(3, r) * v[3];
  }


  #undef M
}
