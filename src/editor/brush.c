#include "editor/editor.h"
#include "editor/brush.h"

#define SIZE_VEC3  sizeof(vec3_t)
#define SIZE_FACE  sizeof(face_t)

#define WINDING_HUGE 32768
#define BRUSHUV_MIN 0.01f
#define BRUSHUV_MAX 10.0f

brush_t* gBrushes = NULL;
u32 gBrushCount = 0;

static inline size_t BrushSize(const brush_t* b)
{
  return sizeof(brush_t) + sizeof(face_t*) * b->numfaces;
}

static inline size_t WindingSize(const winding_t* win)
{
  return sizeof(winding_t) + SIZE_VEC3 * win->pointcount;
}

static inline size_t FaceSize(const face_t* f)
{
  return sizeof(face_t) + WindingSize(f->win) - sizeof(winding_t);
}


winding_t* AllocWinding(const int numpoints)
{
  //printf("AllocWinding\n");
  size_t c = sizeof(winding_t) + SIZE_VEC3 * numpoints;
  winding_t* new = malloc(c);
  if (!new) return NULL;
  memset(new, 0, c);
  new->pointcap = (numpoints >= 3) ? numpoints : 3;
  return new;
}

static u8 WindingAdd(winding_t** in, vec3_t p)
{
 //printf("Winding add\n");
  if ((*in)->pointcount >= MAX_WINDING_POINTS) {
    printf("WindingAdd: too many winding points\n");
    return 0;
  }

  if ((*in)->pointcount >= (*in)->pointcap)
  {
    size_t cap = (*in)->pointcap ? 2 * (*in)->pointcap : 4;
    const size_t c = sizeof(winding_t) + SIZE_VEC3 * cap;
    winding_t* new = realloc(*in, c );
    if (!new) {
      printf("Winding reallocation failed\n");
      return 0;
    }
    *in = new;
  }
  VectorCopy(p, (*in)->points[(*in)->pointcount++]);
  //printf("Point added to winding\n");
  return 1;
}


winding_t* HugeWinding(plane_t plane)
{
  winding_t* out = AllocWinding(4);
  if (!out)
    return NULL;

  vec3_t up;
  if (fabsf(plane.normal[1]) < 0.9f)
    Vector(up, 0, 1, 0); 
  else
    Vector(up, 1, 0, 0);

  vec3_t u, v;
  VectorCrossNorm(up, plane.normal, u);
  VectorCrossNorm(plane.normal, u, v);

  vec3_t centre;
  VectorScale(plane.normal, plane.d, centre);
  VectorScale(u, WINDING_HUGE, u);
  VectorScale(v, WINDING_HUGE, v);

  (out)->points[0][0] = (centre[0] + u[0]) + v[0];
  (out)->points[0][1] = (centre[1] + u[1]) + v[1];
  (out)->points[0][2] = (centre[2] + u[2]) + v[2];

  (out)->points[1][0] = (centre[0] + u[0]) - v[0];
  (out)->points[1][1] = (centre[1] + u[1]) - v[1];
  (out)->points[1][2] = (centre[2] + u[2]) - v[2];

  (out)->points[2][0] = (centre[0] - u[0]) - v[0];
  (out)->points[2][1] = (centre[1] - u[1]) - v[1];
  (out)->points[2][2] = (centre[2] - u[2]) - v[2];

  (out)->points[3][0] = (centre[0] - u[0]) + v[0];
  (out)->points[3][1] = (centre[1] - u[1]) + v[1];
  (out)->points[3][2] = (centre[2] - u[2]) + v[2];
  out->pointcount = 4;
  return out;
}

void ClipWinding(winding_t *in, const plane_t p, winding_t **out)
{
  if (!in || !out)
    return;


  for (int i = 0; i < in->pointcount; i++)
  {
    int j = (i + 1) % in->pointcount;

    vec3_t a, b;
    VectorCopy(in->points[i], a);
    VectorCopy(in->points[j], b);

    const float da = DotProduct(p.normal, a) - p.d;
    const float db = DotProduct(p.normal, b) - p.d;

    const int ina = da <= EPSILON;
    const int inb = db <= EPSILON;

    if (ina && inb)
    {
      WindingAdd(out, b);
    }
    else if (ina && !inb)
    {
      vec3_t dir;
      vec3_t intersection;

      VectorSub(b, a, dir);

      if (PlaneLineIntersection(a, dir, p, intersection, NULL))
        WindingAdd(out, intersection);
    }
    else if (!ina && inb)
    {
      vec3_t dir;
      vec3_t intersection;

      VectorSub(b, a, dir);

      if (PlaneLineIntersection(a, dir, p, intersection, NULL))
        WindingAdd(out, intersection);

      WindingAdd(out, b);
    }
  }
}

face_t* AllocFace(plane_t p, int numpoints)
{
  face_t* new = malloc(sizeof(face_t));
  if (!new) return NULL;
  /*
  new->win = AllocWinding(numpoints);
  if (!new->win) 
  {
    free(new);
    return NULL;
  }
  */
  new->win = NULL;
  memset(&new->material, 0, sizeof(facematerial_t));
  new->material.texhandle = 0;
  new->material.scale[0] = 0.1f;
  new->material.scale[1] = 0.1f; 
  new->plane = p;
  new->changed = 1;
  return new; 
}

void FreeFace(face_t** f)
{
  if (!f || !(*f)) return;
  if ((*f)->win) free((*f)->win); (*f)->win = NULL;
  free(*f); *f = NULL;
}

static void Brush_BuildFace(brush_t* b, const int face)
{
  face_t* subject = b->faces[face];
  winding_t* cur = HugeWinding(subject->plane);

  for (int i = 0; i < b->numfaces; i++)
  {
    if (i == face)
      continue;

    face_t* clip = b->faces[i];
    winding_t* next = AllocWinding(cur->pointcount); // fresh, empty output buffer
    ClipWinding(cur, clip->plane, &next);
    free(cur);
    cur = next;

    if (cur->pointcount == 0)   // fully clipped away, nothing left to clip further
      break;
  }

  subject->win = cur;
  printf("Final = %d\n", cur->pointcount);
}

static void Brush_BuildAllFaces(brush_t* b)
{
  int f;
  for (f = 0; f < b->numfaces; f++)
    Brush_BuildFace(b, f);
}

brush_t* AllocBrush(const int facecount)
{
  size_t c =
    sizeof(brush_t) +
    sizeof(face_t*) * facecount;

  brush_t* brush = calloc(1, c);
  brush->mesh.mesh = NULL;
  if (!brush)
    return NULL;

  brush->numfaces = facecount;
  brush->mesh.mesh = CBaseMesh_Create(facecount * 4, facecount * 4);

  return brush;
}

static void free_brush_surfaces(brush_t** b)
{
  if ( (*b)->mesh.surfaces )
    free((*b)->mesh.surfaces);

  (*b)->mesh.surfaces = NULL;
  (*b)->mesh.surfacecount = 0;
  (*b)->mesh.surfacecapacity = 0;
}

void FreeBrush(brush_t** b)
{
  if (!b || !(*b)) return;
  int f;
  for (f = 0; f < (*b)->numfaces; f++)
    FreeFace(&(*b)->faces[f]);
  (*b)->prev = NULL;
  (*b)->next = NULL;
  free_brush_surfaces(b); 
  CBaseMesh_Destroy((*b)->mesh.mesh);
  free(*b);
}


brush_t* NewBrush(vec3_t mins, vec3_t maxs)
{
  brush_t* new = AllocBrush(6);
  if (!new) return NULL;

  vec3_t mn, mx;
  VectorMins(mins, maxs, mn);
  VectorMaxs(mins, maxs, mx);
  
  float d = VectorDistance(mx, mn);
  if (d < TINY_LIMIT)
  {
    printf("[EDITOR][BRUSH]: Brush is too small\n");
    Brush_Delete(&new);
    return NULL;
  }

  VectorCopy(mn, new->mins);
  VectorCopy(mx, new->maxs);

  new->numfaces = 6;

  plane_t planes[6];
  planes[0].d = -mn[0];
  planes[1].d = mx[0];

  planes[2].d = -mn[1];
  planes[3].d = mx[1];

  planes[4].d = -mn[2];
  planes[5].d = mx[2];

  Vector(planes[0].normal, -1, 0, 0);
  Vector(planes[1].normal, 1, 0, 0);

  Vector(planes[2].normal, 0, -1, 0);
  Vector(planes[3].normal, 0, 1, 0);

  Vector(planes[4].normal, 0, 0, -1);
  Vector(planes[5].normal, 0, 0, 1);

  new->faces[0] = AllocFace(planes[0], 4);
  new->faces[1] = AllocFace(planes[1], 4);

  new->faces[2] = AllocFace(planes[2], 4);
  new->faces[3] = AllocFace(planes[3], 4);

  new->faces[4] = AllocFace(planes[4], 4);
  new->faces[5] = AllocFace(planes[5], 4);
  new->numfaces = 6;

  Brush_BuildAllFaces(new);

  return new;
}

static void Brush_AddTolist(brush_t* b, brush_t** list)
{
  if (!b || !list)
    return;
  b->next = *list;
  if (*list) (*list)->prev = b;
  *list = b;
}

static void Brush_Unlink(brush_t* b)
{
  if (!b) 
    return;
  
  if (b->prev)
    b->prev->next = b->next;

  if (b->next)
    b->next->prev = b->prev;

  b->prev = NULL;
  b->next = NULL;

}


brush_t* Brush_Create(vec3_t mins, vec3_t maxs)
{
  float d = VectorDistance(mins, maxs);
  brush_t* b = NewBrush(mins, maxs);
  if (!b)
    return NULL;
  b->changed = 1;
  Brush_AddTolist(b, &gBrushes);
  gBrushCount++;
  return b;
}

void Brush_Delete(brush_t** b)
{
  if (!b) return;
  Brush_Unlink(*b);
  FreeBrush(b);
  gBrushCount--;
}



void Brush_DeleteAll()
{
  brush_t* a = gBrushes;
  while (a)
  {
    brush_t* next = a->next;
    Brush_Delete(&a);
    a = next;
  }
  gBrushes = NULL;
}


void Brush_SetFaceScale(brush_t* b, int face, float sx, float sy)
{
  if (!b)
    return;

  face_t* f = b->faces[face];
  if (!face)
    return;

  f->material.scale[0] = sx;
  f->material.scale[1] = sy;
  clampf(&f->material.scale[0], BRUSHUV_MIN, BRUSHUV_MAX);
  clampf(&f->material.scale[1], BRUSHUV_MIN, BRUSHUV_MAX);
}


