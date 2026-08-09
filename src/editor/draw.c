#include "editor/brush.h"

#include "engine/shader.h"
#include "editor/editor.h"
#include "platform/window.h"


rsurfbatch_t gSurfbatches[ASSETS_MAX_TEXTURES];

void rsurfbatch_addface(face_t f)
{
  u32 texhandle = f.material.texhandle;
  rsurfbatch_t* batch = &gSurfbatches[texhandle];
  int i;
  if (!batch->mesh)
    batch->mesh = CBaseMesh_Create(f.winding->numpoints, 8);

  winding_t* win = f.winding;
  u32 inds[win->numpoints];
  for (i = 0; i < win->numpoints; i++)
  {
    gpuVertex v;
    VectorCopy(win->points[i], v.xyz);
    VectorCopy(f.plane.normal, v.normal);
    Vector(v.col, 1, 1 ,1);
    FaceUVpoint(&f, win->points[i], v.uv);
    inds[i] = CBaseMesh_PushVertex(batch->mesh, v);
  }
  for (i = 1; i < win->numpoints - 1; i++)
  {
    CBaseMesh_PushTriangleIndices(batch->mesh, inds[0], inds[i], inds[(i+1) % win->numpoints]);
  }
  CBaseMesh_Upload(batch->mesh, GL_STATIC_DRAW);
}

void rsurfbatch_reset(u32 texhandle)
{
  rsurfbatch_t* batch = &gSurfbatches[texhandle];
  if (!batch->mesh) return;
  CBaseMesh_Reset(batch->mesh);
}

void R_DrawSurfbatch(rsurfbatch_t* batch)
{
  if (!batch->mesh) return;
  CBaseMesh_Draw(batch->mesh, GL_TRIANGLES); 
}


// Add a winding to a mesh
static void PushBrushFace(CBaseMesh* mesh, face_t* face)
{
  int v, i;
  int numpoints = face->winding->numpoints;
  uint32_t indices[numpoints];

  for (v = 0; v < numpoints; v++)
  {
    gpuVertex vert = {0};
    VectorCopy(face->plane.normal, vert.normal);
    Vector(vert.col, 1, 1, 1);
    // Temporary UVs
    vert.uv[0] = 0; vert.uv[1] = 0;

    VectorCopy(face->winding->points[v], vert.xyz);

    indices[v] = CBaseMesh_PushVertex(mesh, vert);
    printf("Pushed vertex\n");
  }

  for (i = 1; i < numpoints - 1; i++)
    CBaseMesh_PushTriangleIndices(
        mesh,
        indices[0], indices[i], indices[(i + 1) % numpoints]);
}

uint8_t Brush_UpdateRenderable(brush_t* b)
{
  uint8_t reuse = 0;
  if (!b->renderable)
  {
    b->renderable = calloc(1, sizeof(brushrender_t));
    if (!b->renderable) return 0;
    printf("Renderable created\n");
  }
  if (!b->renderable->mesh)
  {
    b->renderable->mesh = CBaseMesh_Create(6, 6);
    if (!b->renderable->mesh) return 0;
  }
  printf("HERE1\n");

  CBaseMesh_Reset(b->renderable->mesh);
  CBaseMesh* mesh = b->renderable->mesh;
  printf("HERE2\n");



  if (!mesh) return 0;
  int f;
  for (f = 0 ; f < b->facecount; f++)
    PushBrushFace(mesh, &b->faces[f]);
  
  CBaseMesh_Upload(mesh, GL_STATIC_DRAW);
  return 1; 
}




void R_DrawBrush(brush_t* b)
{
  CBaseMesh* mesh = b->renderable->mesh;
  CBaseMesh_Draw(mesh, GL_TRIANGLES);
}

static inline void glViewportRect(rectdef rect)
{
  glViewport(rect[0],
      gPltWindow->winh - (rect[1] + rect[3]),
      rect[2], rect[3]
      );
}

void R_DrawBrushes(brush_t* list)
{
  glViewportRect(gPanels[PANEL_3D].rect);
  brush_t* b = NULL;
  for (b = list; b; b = b->next)
    R_DrawBrush(b);

  glViewport(0,0, gPltWindow->winw, gPltWindow->winh);
}


void R_DrawSurfaces()
{
  // optimise by using only known textures that are in use
  glViewportRect(gPanels[PANEL_3D].rect);
  for (int i = 0; i < ASSETS_MAX_TEXTURES; i++)
  {
    R_DrawSurfbatch(&gSurfbatches[i]);
  }
  glViewport(0,0, gPltWindow->winw, gPltWindow->winh);
}
