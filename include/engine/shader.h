#pragma once

#include <glad/glad.h>
#include <assert.h>
#include "corebase/vectors.h"

// TODO: Cache shader uniform locations

typedef enum CBASE_SHADER_LOC_TYPE
{

  // Matrices
  SH_UNIFORM_MODEL,
  SH_UNIFORM_VIEW,
  SH_UNIFORM_PROJECTION,
  
  // Sampler
  SH_UNIFORM_TEXTURE,

  // Vectors
  SH_UNIFORM_COLOUR,


  // Booleans/ints
  SH_UNIFORM_USE_TEXUTRE,
  SH_UNIFORM_USE_VERTEXCOL,
  SH_UNIFORM_USE_NORMALS,

  // Other uniforms ...

  

  SH_UNIFORM_COUNT // Not a uniform - counter, must be last

} CBaseShaderLocType;


typedef struct CBaseShader
{
  GLuint program;
  const char* srcVertex;
  const char* srcFrag;
  
  GLint uniforms[SH_UNIFORM_COUNT];

} CBaseShader;

static GLuint CBaseShader_GetLoc( CBaseShader* shader, CBaseShaderLocType loctype )
{
  assert(shader);
  return shader->uniforms[loctype];
}

void          CBaseShader_Destroy       (CBaseShader* shader);
void          CBaseShader_Use           (CBaseShader* shader);

void          CBaseShader_PrintUniforms (CBaseShader* shader );

CBaseShader*  CBaseShader_Create        (const char* vert, const char* frag);


void          CBaseShader_SetMat4( CBaseShader* shader, CBaseShaderLocType loctype, mat4 m);
