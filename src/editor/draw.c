#include "editor/brush.h"
#include "editor/brushmesh.h"
#include "editor/editor.h"
#include "engine/mesh.h"
#include "platform/window.h"

#include "engine/hash.h"

static int facecmp(const void* x, const void* y)
{
  face_t** a = (face_t**)(x);
  face_t** b = (face_t**)(y);

  if ((*a)->material.texhandle < (*b)->material.texhandle)
    return -1;
  if ((*a)->material.texhandle > (*b)->material.texhandle)
    return 1;
  return 0;
}

static inline void sort_brush_faces(brush_t* b)
{
  qsort(b->faces, b->numfaces, sizeof(face_t*), facecmp);
  printf("Faces sorted\n");
}



static void plane_uvaxis(plane_t p, vec3_t u, vec3_t v)
{
vec3_t up;
  if (fabsf(p.normal[1]) < 0.9f)
    Vector(up, 0, 1, 0);
  else
    Vector(up, 1, 0, 0);

  VectorCrossNorm(up, p.normal, u);
  VectorCrossNorm(p.normal, u, v);

}

static void face_uvs(face_t* f, vec3_t p, vec2_t out)
{
  float sx = f->material.scale[0];
  float sy = f->material.scale[1];

  float u = DotProduct(p, f->material.uaxis) * sx;
  float v = DotProduct(p, f->material.vaxis) * sy;

  out[0] = u + f->material.shift[0];
  out[1] = v + f->material.shift[1];
  clampf(&f->material.scale[0], 0.1f, 0.5f);
  clampf(&f->material.scale[1], 0.1f, 0.5f);
}




static void update_mesh(brush_t* b)
{
  sort_brush_faces(b);
  int i, j;
  brushmesh_t* mesh = &b->mesh;
  mesh->surfacecount = 0;
  
  if (!mesh->surfaces || b->numfaces > mesh->surfacecapacity)
  {
    mesh->surfaces = realloc(mesh->surfaces, sizeof(brushsurface_t) * b->numfaces);
    mesh->surfacecapacity = b->numfaces;
  }
  if (!mesh->mesh) // Init the mesh
    mesh->mesh = CBaseMesh_Create(24, 24);
  CBaseMesh_Reset(mesh->mesh);
  
  texbucketent_t* entries[BRUSHMESH_TEXHASH_BUCKETS] = {0};


  for (i = 0; i < b->numfaces; i++)
  {
    face_t* face = b->faces[i];
    u32 texhandle = face->material.texhandle;
    u32 bucket = Hash_Bucket( Hash_Int(texhandle), BRUSHMESH_TEXHASH_BUCKETS );
    brushsurface_t* surf = NULL;

    // Find correct surface (avoid collisions)
    for (texbucketent_t* e = entries[bucket]; e ; e = e->next)
    {
      if (e->texhandle == texhandle)
      {
        surf = &mesh->surfaces[e->surface_index];
        break;
      }
    }

    // No surface found -> new texture
    if (!surf)
    {
      int newidx = mesh->surfacecount++;
      surf = &mesh->surfaces[newidx];
      surf->firstindex = mesh->mesh->indexcount;
      surf->indexcount = 0;
      surf->texhandle = texhandle;

      texbucketent_t* entry = malloc(sizeof(texbucketent_t));
      entry->texhandle = surf->texhandle;
      entry->surface_index = newidx;
      entry->next = entries[bucket];
      entries[bucket] = entry;
    }


    if (face->changed)
    {
      plane_uvaxis(face->plane, face->material.uaxis, face->material.vaxis);
      face->changed = 0;
    }
    winding_t* win = face->win;
    u32 indices[win->pointcount];
    for (j = 0; j < win->pointcount; j++)
    {
      gpuVertex vertex;
      VectorCopy(win->points[j], vertex.xyz);
      VectorCopy(face->plane.normal, vertex.normal);
      Vector4(vertex.col, 1, 1, 1, 1);
      face_uvs(face, win->points[j], vertex.uv);
      indices[j] = CBaseMesh_PushVertex(mesh->mesh, vertex);
    }

    for (j = 1; j + 1 < win->pointcount; j++)
    {
      u32 i0 = indices[0];
      u32 i1 = indices[j];
      u32 i2 = indices[j + 1];
      CBaseMesh_PushTriangleIndices(mesh->mesh, i0, i1, i2);
      surf->indexcount+=3;
    }
  }

  // Free the hash map
  for (int bucketcount = 0; bucketcount < BRUSHMESH_TEXHASH_BUCKETS; bucketcount++)
  {
    texbucketent_t* e = entries[bucketcount];
    while (e) {texbucketent_t* next = e->next; free(e); e = next;}
  }


  CBaseMesh_Upload(mesh->mesh, GL_STATIC_DRAW);
  CBaseMesh_Print(mesh->mesh);
  printf("[EDITOR][BRUSH]: Mesh uploaded\n");
}

void R_DrawFaceHighlight() {
  if (!gFaceHighlight.mesh) return;
  CBaseMesh_Draw(gFaceHighlight.mesh, GL_TRIANGLES);
}

void R_DrawBrush(brush_t* b)
{
  if  (b->changed)
  {
    Brush_BuildAllFaces(b);
    update_mesh(b);
    b->changed = 0;
  }


  int i;
  brushsurface_t* surface;
  int texhandle = -1;
  CBaseMesh* mesh = b->mesh.mesh;
  if (!mesh)
    return;

  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(mesh->vao);
  for (i = 0; i < b->mesh.surfacecount; i++)
  {
    surface = &b->mesh.surfaces[i];
    if (surface->texhandle != texhandle)
    {
      GLuint texid = gAssetManager->textures.tex[surface->texhandle].texture.gltexnum;
      //printf("TEX id = %du\n", texid);
      glBindTexture(GL_TEXTURE_2D, texid);
      CBaseShader_SetInt(gEditorShader_brush, SH_UNIFORM_USE_TEXTURE, 1);
      CBaseShader_SetInt(gEditorShader_brush, SH_UNIFORM_TEXTURE, 0);
      texhandle = surface->texhandle;
    }

    glDrawElements(
        GL_TRIANGLES,
        (GLsizei)surface->indexcount,
        GL_UNSIGNED_INT,
        (void*)(surface->firstindex * sizeof(u32))
    );
  }
  glBindVertexArray(0); 
}


static inline void ViewportRect(rectdef rect)
{
  glViewport(
      rect[0],
      gPltWindow->winh - (rect[1] + rect[3]),
      rect[2],
      rect[3]
      );
}

/*
void R_DrawBrush(brush_t* b)
{
  if (b->changed)
  {
    update_mesh(b);
    b->changed = 0;
  }

  CBaseMesh_Draw(b->mesh.mesh, GL_TRIANGLES);
}
*/


void R_DrawBrushes()
{
  ViewportRect(gPanels[PANEL_3D].rect);
  brush_t* b = NULL;
  CBaseShader_SetInt(gEditorShader_brush, SH_UNIFORM_USE_TEXTURE, 1);
  for (b = gBrushes; b; b = b->next)
  {
    R_DrawBrush(b);
  }
  if (gHoveredBrush)
    R_DrawFaceHighlight();
  glViewport(0, 0, gPltWindow->winw, gPltWindow->winh);
}
