#include "editor/brush.h"
#include <stdio.h>

#include "engine/mesh.h"
#define TINY_LIMIT 0.01F

brush_t* gBrushes = NULL;

winding_t* NewWinding(int numpoints)
{
  if (numpoints >= WINDING_MAX_POINTS)
  {
    printf("[BSP][WINDING]: Attempted to make winding of too many points, (%d/%d)\n", numpoints, WINDING_MAX_POINTS);
  }
  size_t c = sizeof(winding_t) + sizeof(vec3_t) * numpoints;
  winding_t* win = malloc(c);
  if (!win) return NULL;
  memset(win, 0, sizeof(*win));

  win->numpoints = numpoints;
  win->maxpoints = numpoints;
  return win;
}

void FreeWinding(winding_t* win)
{
  if (win) free(win);
}


winding_t* HugeWinding( plane_t p )
{
  winding_t* win = NewWinding(4);
  if (!win) return NULL;
  
  float size = 100000.0f;
  vec3_t up, u, v, centre;
  if (fabsf(p.normal[1]) < 0.9f)
    Vector(up, 0, 1, 0);
  else
    Vector(up, 1, 0, 0);

  VectorCrossNorm(up, p.normal, u);
  VectorCrossNorm(p.normal, u, v);

  VectorCopy(p.normal, centre);
  VectorScale(centre, p.d);
  
  VectorScale(u, size);
  VectorScale(v, size);


  // CCW
  VectorSub(centre, u, win->points[0]);
  VectorSub(win->points[0], v, win->points[0]);

  VectorAdd(centre, u, win->points[1]);
  VectorSub(win->points[1], v, win->points[1]);

  VectorAdd(centre, u, win->points[2]);
  VectorAdd(win->points[2], v, win->points[2]);

  VectorSub(centre, u, win->points[3]);
  VectorAdd(win->points[3], v, win->points[3]);

  return win;
}


face_t* NewFace( plane_t p )
{
  face_t* f = malloc(sizeof(face_t));
  memset(f, 0, sizeof(face_t));
  f->plane = p;
  return f;
}

void FreeFace( face_t* f )
{
  if (!f) return;
  FreeWinding(f->winding);
  free(f);
  f = NULL;
}


// Allocate a new brush to hold 'facecount' brushes
brush_t* NewBrush( int facecount )
{
  /*
  size_t c = sizeof(brush_t) + facecount * sizeof(face_t);
  brush_t* b = malloc(c);
  b->facecount = 0;
  b->next = NULL; b->prev = NULL;
  */
  brush_t* b = calloc(1, sizeof(brush_t));
  b->faces = malloc(sizeof(face_t*) * facecount);
  memset(b->faces, 0, sizeof(face_t*) * facecount);
  b->facecount = 0;
  b->facecapacity = facecount;
  return b;
}

void Brush_Delete(brush_t* b)
{
  free(b->faces);
  b->facecount = 0;
  b->facecapacity = 0;
  b->next = b->prev = NULL;
  free(b);
}


u8 BoundsTiny(vec3_t mins, vec3_t maxs)
{
  return VectorDistance(mins, maxs) < TINY_LIMIT;
}

void ClipWinding(winding_t* win, plane_t p, winding_t* out)
{
  if (!win || !out)
    exit(1);
  int i;
  for (i = 0; i < win->numpoints; i++)
  {
    vec3_t a, b;
    vec_t da, db;
    VectorCopy(win->points[i], a);
    VectorCopy(win->points[(i+1) % win->numpoints], b);

    da = DotProduct(p.normal, a) - p.d;
    db = DotProduct(p.normal, b) - p.d;

    int ina = (da < -EPSILON);
    int inb = (db < -EPSILON);

    if (ina)
      VectorCopy(a, out->points[out->numpoints++]);
  
    if (ina != inb)
    {
      // Find plane intersection
      float t = da / (da - db);
      vec3_t hit;
      hit[0] = a[0] + t * (b[0] - a[0]);
      hit[1] = a[1] + t * (b[1] - a[1]);
      hit[2] = a[2] + t * (b[2] - a[2]);
      VectorCopy(hit, out->points[out->numpoints++]);
    }
  }
}

u8 Brush_AddToList(brush_t* b, brush_t* list)
{
  if (!b || !list)
    return 0;

 list->next = b;
  b->prev = list;

  return 1;
}

u8 Brush_RemoveFromList(brush_t* b, brush_t* list)
{
  if (!b || !list)
    return 0;

  list->next = b;
  b->prev = list;

  return 1;
}

u8 Brush_AddFace(brush_t* b, face_t* f)
{
  if (!b || !f)
    return 0;
  
  if (b->facecount >= BRUSH_MAX_PLANES)
    return 0;
  
  if (b->facecount >= b->facecapacity)
  {
    b->facecapacity = b->facecapacity ? b->facecapacity * 2 : 6;
    b->faces = realloc(b->faces, sizeof(face_t) * b->facecapacity);
  }
  b->faces[b->facecount++] =  *f;
    
  return 1;
}

/*
u8 Brush_AddFace(brush_t* b, face_t* f)
{
  if (!b || !f)
    return 0;

  if (b->facecount >= BRUSH_MAX_PLANES)
    return 0;

  b->faces[b->facecount++] = *f;
  return 1;
}
*/

// Returns the max x,y,z of a and b
static inline void VectorMaxs(vec3_t a, vec3_t b, vec3_t out)
{
  out[0] = fmaxf(a[0], b[0]);
  out[1] = fmaxf(a[1], b[1]);
  out[2] = fmaxf(a[2], b[2]);
}
// Returns the min x,y,z of a and b
static inline void VectorMins(vec3_t a, vec3_t b,  vec3_t out)
{
  out[0] = fminf(a[0], b[0]);
  out[1] = fminf(a[1], b[1]);
  out[2] = fminf(a[2], b[2]);
}




void Face_BuildWinding(brush_t* b, u16 index)
{
  // Compare against the rest of the brush faces
  face_t f = b->faces[index];
  f.winding = HugeWinding(f.plane);
  // Clip against the rest of the brush planes
  for (int i = 0; i < b->facecount; i++)
  {
    if (i == index)
      continue;
    plane_t clip = b->faces[i].plane;
    ClipWinding(f.winding, clip, f.winding);
  }
}

static void Brush_ComputeBounds(brush_t* b)
{
  if (!b) return;
  vec3_t mins, maxs;
  Vector(mins, 0, 0, 0);
  Vector(maxs, 0, 0, 0);
  for (int i = 0; i < b->facecount; i++)
  {
    for (int j = 0; j < b->faces[i].winding->numpoints; j++)
    {
      vec3_t point;
      VectorCopy(b->faces[i].winding->points[j], point);
      VectorMins(point, mins, mins);
      VectorMaxs(point, maxs, maxs);
    }
  }
  VectorCopy(mins, b->mins);
  VectorCopy(maxs, b->maxs);
}

void Brush_Build(brush_t* b)
{
  for (int i = 0; i < b->facecount; i++)
  {
    Face_BuildWinding(b, i);
  }
  Brush_ComputeBounds(b);
}

static void Brush_CreateRenderable(brush_t* b)
{
  if (!b) return;
  b->renderable = calloc(1, sizeof(brushrender_t));
  b->renderable->mesh = CBaseMesh_Create(6, 12);
  b->renderable->old = 1;
}

static void Brush_UpdateRenderable(brush_t* b)
{
  if (!b) return;
  if (!b->renderable) return;
  
  int i, j;
  for (i = 0; i < b->facecount; i++)
  {
    face_t* f = &b->faces[i];
    for (j = 0; j < f->winding->numpoints; j++)
    {
      gpuVertex v = {0};
      VectorCopy(f->plane.normal, v.normal);
      VectorCopy(f->winding->points[j], v.xyz);
// TODO: FINSIH HERE
    }
  }

}

brush_t* Brush_Create(vec3_t mins, vec3_t maxs)
{
  if (BoundsTiny(mins, maxs))
    return 0;
  // Ensure mins and maxes
  vec3_t mn, mx;
  VectorMaxs(mins, maxs, mx);
  VectorMins(mins, maxs, mn);
    
  brush_t* out = NewBrush(6);
  // Planes point outwards
  plane_t xmin, xmax;
  xmin.d = mn[0]; xmax.d = mx[0];
  Vector(xmin.normal, -1, 0, 0);
  Vector(xmax.normal,  1, 0, 0);

  plane_t ymin, ymax;
  ymin.d = mn[1]; ymax.d = mx[1];
  Vector(ymin.normal,  0, -1, 0);
  Vector(ymax.normal,  0,  1, 0);

  plane_t zmin, zmax;
  zmin.d = mn[2]; zmax.d = mx[2];
  Vector(zmin.normal, 0, 0, -1);
  Vector(zmax.normal, 0, 0, 1);
  
  face_t* f0 = NewFace(xmin);
  face_t* f1 = NewFace(xmax); 
  face_t* f2 = NewFace(ymin);
  face_t* f3 = NewFace(ymax);
  face_t* f4 = NewFace(zmin);
  face_t* f5 = NewFace(zmax);
    
  Brush_AddFace(out, f0);
  Brush_AddFace(out, f1);
  Brush_AddFace(out, f2);
  Brush_AddFace(out, f3);
  Brush_AddFace(out, f4);
  Brush_AddFace(out, f5);

  // Build brush
  Brush_Build(out);

  printf("[EDITOR][BRUSH]: Brush created\n");
  return out;
}





