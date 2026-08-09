
#include "editor/brush.h"
#include "editor/editor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>

#include "engine/mesh.h"

#define TINY_LIMIT 0.01F

brush_t* gBrushes = NULL;

winding_t* NewWinding(void)
{
  winding_t* win = malloc(sizeof(*win));
  if (!win)
    return NULL;

  memset(win, 0, sizeof(*win));
  return win;
}

void FreeWinding(winding_t* win)
{
  free(win);
}

winding_t* HugeWinding(plane_t p)
{
  winding_t* win = NewWinding();
  if (!win)
    return NULL;

  float size = 100000.0f;
  vec3_t up, u, v, centre;

  if (fabsf(p.normal[1]) < 0.9f)
    Vector(up, 0, 1, 0);
  else
    Vector(up, 1, 0, 0);

  VectorCrossNorm(up, p.normal, u);
  VectorCrossNorm(p.normal, u, v);

  VectorCopy(p.normal, centre);
  VectorScale(centre, p.d, centre);

  VectorScale(u, size, u);
  VectorScale(v, size, v);

  VectorSub(centre, u, win->points[0]);
  VectorSub(win->points[0], v, win->points[0]);

  VectorAdd(centre, u, win->points[1]);
  VectorSub(win->points[1], v, win->points[1]);

  VectorAdd(centre, u, win->points[2]);
  VectorAdd(win->points[2], v, win->points[2]);

  VectorSub(centre, u, win->points[3]);
  VectorAdd(win->points[3], v, win->points[3]);

  win->numpoints = 4;

  return win;
}

static void FaceUVBasis(face_t *f)
{
  brushmaterial_t *mat = &f->material;

  mat->shift[0] = 0;
  mat->shift[1] = 0;

  mat->scale[0] = 1;
  mat->scale[1] = 1;

  if (f->plane.normal[0] > 0.9f)
  {
    Vector(mat->uaxis, 0, 1, 0);
    Vector(mat->vaxis, 0, 0, 1);
  }
  else if (f->plane.normal[0] < -0.9f)
  {
    Vector(mat->uaxis, 0, -1, 0);
    Vector(mat->vaxis, 0, 0, 1);
  }
  else if (f->plane.normal[1] > 0.9f)
  {
    Vector(mat->uaxis, 1, 0, 0);
    Vector(mat->vaxis, 0, 0, 1);
  }
  else if (f->plane.normal[1] < -0.9f)
  {
    Vector(mat->uaxis, -1, 0, 0);
    Vector(mat->vaxis, 0, 0, 1);
  }
  else if (f->plane.normal[2] > 0.9f)
  {
    Vector(mat->uaxis, 1, 0, 0);
    Vector(mat->vaxis, 0, 1, 0);
  }
  else
  {
    Vector(mat->uaxis, 1, 0, 0);
    Vector(mat->vaxis, 0, -1, 0);
  }
}

void FaceUVpoint(face_t* f, vec3_t p, vec2_t out)
{
  const float scale = 0.01f;
  brushmaterial_t* mat = &f->material;
  float u = DotProduct(p, mat->uaxis) * scale;
  float v = DotProduct(p, mat->vaxis) * scale;
  u *= mat->scale[0];
  v *= mat->scale[1];

  out[0] = u + mat->shift[0];
  out[1] = v + mat->shift[1];
}


face_t NewFace(plane_t p)
{
  face_t f = {0};
  memset(&f.material, 0, sizeof(f.material));
  f.material.texhandle = 1;
  f.plane = p;
  return f;
}

brush_t* NewBrush(int facecount)
{
  (void)facecount;

  brush_t* b = calloc(1, sizeof(*b));
  if (!b)
    return NULL;

  return b;
}

void Brush_Delete(brush_t* b)
{
  if (!b)
    return;

  for (u32 i = 0; i < b->facecount; i++)
  {
    FreeWinding(b->faces[i].winding);
    b->faces[i].winding = NULL;
  }

  b->facecount = 0;
  b->next = NULL;
  b->prev = NULL;

  free(b);
}

u8 BoundsTiny(vec3_t mins, vec3_t maxs)
{
  return VectorDistance(mins, maxs) < TINY_LIMIT;
}

void ClipWinding(winding_t* win, plane_t p, winding_t* out)
{
  if (!win || !out)
    return;

  winding_t temp = {0};

  if (win->numpoints <= 0)
  {
    out->numpoints = 0;
    return;
  }

  for (int i = 0; i < win->numpoints; i++)
  {
    vec3_t a, b;
    vec_t da, db;

    VectorCopy(win->points[i], a);
    VectorCopy(win->points[(i + 1) % win->numpoints], b);

    da = DotProduct(p.normal, a) - p.d;
    db = DotProduct(p.normal, b) - p.d;

    int ina = da <= EPSILON;
    int inb = db <= EPSILON;

    if (ina)
    {
      if (temp.numpoints >= WINDING_MAX_POINTS)
        break;

      VectorCopy(a, temp.points[temp.numpoints++]);
    }

    if (ina != inb)
    {
      float denom = da - db;

      if (fabsf(denom) > EPSILON)
      {
        float t = da / denom;
        vec3_t hit;

        hit[0] = a[0] + t * (b[0] - a[0]);
        hit[1] = a[1] + t * (b[1] - a[1]);
        hit[2] = a[2] + t * (b[2] - a[2]);

        if (temp.numpoints >= WINDING_MAX_POINTS)
          break;

        VectorCopy(hit, temp.points[temp.numpoints++]);
      }
    }
  }

  *out = temp;
}

u8 Brush_AddToList(brush_t* b, brush_t** list)
{
  if (!b || !list)
    return 0;

  b->prev = NULL;
  b->next = *list;

  if (*list)
    (*list)->prev = b;

  *list = b;

  return 1;
}

u8 Brush_RemoveFromList(brush_t* b, brush_t** list)
{
  if (!b || !list)
    return 0;

  if (b->prev)
    b->prev->next = b->next;
  else if (*list == b)
    *list = b->next;

  if (b->next)
    b->next->prev = b->prev;

  b->next = NULL;
  b->prev = NULL;

  return 1;
}

u8 Brush_AddFace(brush_t* b, face_t f)
{
  if (!b)
    return 0;

  if (b->facecount >= BRUSH_MAX_PLANES)
    return 0;

  b->faces[b->facecount++] = f;
  return 1;
}

static inline void VectorMaxs(vec3_t a, vec3_t b, vec3_t out)
{
  out[0] = fmaxf(a[0], b[0]);
  out[1] = fmaxf(a[1], b[1]);
  out[2] = fmaxf(a[2], b[2]);
}

static inline void VectorMins(vec3_t a, vec3_t b, vec3_t out)
{
  out[0] = fminf(a[0], b[0]);
  out[1] = fminf(a[1], b[1]);
  out[2] = fminf(a[2], b[2]);
}

static void Brush_PrintVertices(brush_t* b)
{
  if (!b)
    return;

  for (u32 i = 0; i < b->facecount; i++)
  {
    face_t* f = &b->faces[i];

    if (!f->winding)
      continue;

    for (int v = 0; v < f->winding->numpoints; v++)
      vec3print(f->winding->points[v]);
  }
}

void Face_BuildWinding(brush_t* b, u16 index)
{
  if (!b || index >= b->facecount)
    return;

  face_t* f = &b->faces[index];

  FreeWinding(f->winding);
  f->winding = HugeWinding(f->plane);

  if (!f->winding)
    return;

  for (u32 i = 0; i < b->facecount; i++)
  {
    if (i == index)
      continue;

    if (f->winding->numpoints == 0)
      break;

    winding_t clipped = {0};

    ClipWinding(
      f->winding,
      b->faces[i].plane,
      &clipped
    );

    *f->winding = clipped;
  }
  FaceUVBasis(f);
}

static void Brush_ComputeBounds(brush_t* b)
{
  if (!b)
    return;

  vec3_t mins;
  vec3_t maxs;

  Vector(mins, FLT_MAX, FLT_MAX, FLT_MAX);
  Vector(maxs, -FLT_MAX, -FLT_MAX, -FLT_MAX);

  int found = 0;

  for (u32 i = 0; i < b->facecount; i++)
  {
    winding_t* winding = b->faces[i].winding;

    if (!winding)
      continue;

    for (int j = 0; j < winding->numpoints; j++)
    {
      VectorMins(winding->points[j], mins, mins);
      VectorMaxs(winding->points[j], maxs, maxs);
      found = 1;
    }
  }

  if (!found)
  {
    Vector(b->mins, 0, 0, 0);
    Vector(b->maxs, 0, 0, 0);
    return;
  }

  VectorCopy(mins, b->mins);
  VectorCopy(maxs, b->maxs);
}


void Brush_Build(brush_t *b)
{
  if (!b)
    return;

  for (u32 i = 0; i < ASSETS_MAX_TEXTURES; i++)
    rsurfbatch_reset(i);

  for (u32 i = 0; i < b->facecount; i++)
    Face_BuildWinding(b, i);

  for (u32 i = 0; i < b->facecount; i++)
    rsurfbatch_addface(b->faces[i]);

  Brush_ComputeBounds(b);
}

brush_t* Brush_Create(vec3_t mins, vec3_t maxs)
{
  if (BoundsTiny(mins, maxs))
  {
    printf("BOUNDS TINY\n");
    return NULL;
  }

  vec3_t mn, mx;

  VectorMins(mins, maxs, mn);
  VectorMaxs(mins, maxs, mx);

  brush_t* out = NewBrush(6);

  if (!out)
    return NULL;

  plane_t xmin, xmax;
  xmin.d = -mn[0];
  xmax.d = mx[0];

  Vector(xmin.normal, -1, 0, 0);
  Vector(xmax.normal, 1, 0, 0);

  plane_t ymin, ymax;
  ymin.d = -mn[1];
  ymax.d = mx[1];

  Vector(ymin.normal, 0, -1, 0);
  Vector(ymax.normal, 0, 1, 0);

  plane_t zmin, zmax;
  zmin.d = -mn[2];
  zmax.d = mx[2];

  Vector(zmin.normal, 0, 0, -1);
  Vector(zmax.normal, 0, 0, 1);

  Brush_AddFace(out, NewFace(xmin));
  Brush_AddFace(out, NewFace(xmax));
  Brush_AddFace(out, NewFace(ymin));
  Brush_AddFace(out, NewFace(ymax));
  Brush_AddFace(out, NewFace(zmin));
  Brush_AddFace(out, NewFace(zmax));

  Brush_Build(out);
  Brush_UpdateRenderable(out);

  Brush_PrintVertices(out);

  printf("[EDITOR][BRUSH]: Brush created\n");

  return out;
}

void Brush_FreeList(brush_t* list)
{
  while (list)
  {
    brush_t* next = list->next;
    Brush_Delete(list);
    list = next;
  }
}

