#include "corebase/mesh.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))


static CBaseMesh* CBaseMesh_Alloc(int vertexcap, int indexcap)
{
  if (vertexcap <= 0 || indexcap <= 0)
    return NULL;

  CBaseMesh* new = malloc(sizeof(CBaseMesh));
  memset(new, 0, sizeof(CBaseMesh));

  new->vertices = malloc(sizeof(gpuVertex) * vertexcap);
  new->indices = malloc(sizeof(GLuint) * indexcap);

  new->vertexcap = vertexcap;
  new->indexcap = indexcap;

  if (!new->vertices || !new->indices)
    return NULL;


  return new;
}


void CBaseMesh_Destroy(CBaseMesh* mesh)
{
  assert(mesh);
  free(mesh->indices);
  free(mesh->vertices);
  memset(mesh, 0, sizeof(CBaseMesh));
  free(mesh);
}

static void GenerateBuffers(CBaseMesh* mesh)
{
  glGenVertexArrays(1, &mesh->vao);
  glGenBuffers(1, &mesh->vbo);
  glGenBuffers(1, &mesh->ebo);
}


CBaseMesh* CBaseMesh_Create(int vbocap, int ebocap)
{
  CBaseMesh* new = CBaseMesh_Alloc(vbocap, ebocap);
  if (!new)
  {
    printf("[MESH]: Failed to allocate new mesh\n");
    return NULL;
  }
  GenerateBuffers(new);

  return new;
}


void CBaseMesh_Upload(CBaseMesh* mesh, GLenum usage)
{
  assert(mesh);

  glBindVertexArray(mesh->vao);
  // VBO
  glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
  glBufferData(GL_ARRAY_BUFFER, mesh->vertexcount * sizeof(gpuVertex), mesh->vertices, usage);
  // EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indexcount * sizeof(GLuint), mesh->indices, usage);

  // xyz
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(gpuVertex), (void*)OFFSETOF(gpuVertex, xyz)); 
  // normal
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(gpuVertex), (void*)OFFSETOF(gpuVertex, normal));
  // colour
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(gpuVertex), (void*)OFFSETOF(gpuVertex, col));
  // uv
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(gpuVertex), (void*)OFFSETOF(gpuVertex, uv));


  glBindVertexArray(0);
}

void CBaseMesh_Draw(CBaseMesh* mesh, GLenum primitive)
{
  glBindVertexArray(mesh->vao);
  glDrawElements(primitive, (GLsizei)mesh->indexcount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

}

unsigned int CBaseMesh_PushVertex(CBaseMesh* mesh, gpuVertex v)
{
  assert(mesh);
  
  if (mesh->vertexcount >= mesh->vertexcap)
  {
    // Grow vertex array
    mesh->vertexcap *= 2;
    mesh->vertices = realloc(
        mesh->vertices,
        sizeof(gpuVertex) * mesh->vertexcap
    );
  }
  mesh->vertices[mesh->vertexcount] = v;
  return mesh->vertexcount++;
}

void CBaseMesh_PushTriangleIndices(

    CBaseMesh* mesh,
    GLuint i0,
    GLuint i1,
    GLuint i2
)
{
  assert(mesh && mesh->indices);
  
  // Grow index array
  if (mesh->indexcount >= mesh->indexcap)
  {
    mesh->indexcap *= 2;
    mesh->indices = realloc(
        mesh->indices,
        sizeof(GLuint) * mesh->indexcap
        );
  }

  mesh->indices[mesh->indexcount++] = i0;
  mesh->indices[mesh->indexcount++] = i1;
  mesh->indices[mesh->indexcount++] = i2;
}


void CBaseMesh_PushTriangleVerts(
    CBaseMesh* mesh, 
    gpuVertex v0,
    gpuVertex v1,
    gpuVertex v2
)
{
  assert(mesh);

  GLuint i0 = CBaseMesh_PushVertex(mesh, v0);
  GLuint i1 = CBaseMesh_PushVertex(mesh, v1);
  GLuint i2 = CBaseMesh_PushVertex(mesh, v2);
  
  CBaseMesh_PushTriangleIndices(mesh, i0, i1, i2);
}


