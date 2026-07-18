#pragma once

#include <glad/glad.h>

// GPU vertex data
typedef struct CBASE_VERTEX
{
  float xyz[3];
  float normal[3];
  float col[3];
  float uv[2];
} gpuVertex;

typedef struct CBASE_MESH
{
  GLuint vao;
  GLuint vbo;
  GLuint ebo;

  gpuVertex* vertices;
  GLuint*    indices;

  int vertexcount;
  int indexcount;

  int vertexcap;
  int indexcap;

} CBaseMesh;


CBaseMesh*    CBaseMesh_Create(int vertexcount, int indexcount);

void          CBaseMesh_Destroy(CBaseMesh* mesh);

unsigned int  CBaseMesh_PushVertex(CBaseMesh* mesh, gpuVertex v);


void CBaseMesh_PushTriangleIndices(
    CBaseMesh* mesh, 
    GLuint i0, 
    GLuint i1, 
    GLuint i2
);

void CBaseMesh_PushTriangleVerts(
    CBaseMesh* mesh,
    gpuVertex v0,
    gpuVertex v1,
    gpuVertex v2
);



#define CBaseMesh_PushTriangle(mesh, a, b, c) \
  _Generic((a),                   \
    gpuVertex: CBaseMesh_PushTriangleVerts, \
    GLuint:    CBaseMesh_PushTriangleIndices \
    unsigned int: CBaseMesh_PushTriangleIndices \
  )(mesh, a, b, c)


void CBaseMesh_Upload(CBaseMesh* mesh, GLenum usage);
void CBaseMesh_Draw(CBaseMesh* mesh, GLenum primitive);
