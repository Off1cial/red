#include "editor/brush.h"

#define BRUSH_FRONT 1
#define BRUSH_BACK  2
#define BRUSH_CROSS 4


static inline size_t BrushSize(const brush_t* b)
{
  return sizeof(brush_t) + sizeof(face_t*) * b->numfaces;
}

static inline size_t WindingSize(const winding_t* win)
{
  return sizeof(winding_t) + (sizeof(vec3_t)) * win->pointcount;
}

static inline size_t FaceSize(const face_t* f)
{
  return sizeof(face_t) + WindingSize(f->win) - sizeof(winding_t);
}

static inline bool WindingValid(winding_t* in)
{
  return in->pointcount >= 3;
}

static winding_t* WindingEmpty()
{
  winding_t* out = malloc(sizeof(winding_t));
  out->pointcap = 1;
  out->pointcount = 0;
  return out;
}

static void Brush_AddFace(brush_t** in, face_t* f)
{
    size_t newsize =
        sizeof(brush_t) +
        sizeof(face_t*) * ((*in)->numfaces + 1);

    brush_t* newbrush = realloc(*in, newsize);

    if (!newbrush)
    {
        printf("JEKSGJSGJSGJSPJGS\n");
        exit(1);
    }

    newbrush->faces[newbrush->numfaces] = f;
    newbrush->numfaces++;

    *in = newbrush;
}

void Brush_Splice(brush_t* in, const plane_t split, brush_t** front, brush_t** back)
{
  *front = NULL; *back = NULL;
  // Dictate where the brush lies relative to the plane
  int8_t posbits = 0;
  for (int i = 0; i < in->numfaces; i++)
  {
    winding_t* win = in->faces[i]->win;
    for (int j = 0; j < win->pointcount; j++)
    {
      // Determine where each point lies 
      float d = DotProduct(split.normal, win->points[j]) - split.d;
      if ( d > EPSILON )
        posbits |= BRUSH_FRONT;
      if (d < -EPSILON)
        posbits |= BRUSH_BACK;
    }
  }

  // Does the brush lie entirely on one side
  if (!(posbits & BRUSH_BACK)) // Entirely on the front
  {
    *front = AllocBrush(in->numfaces);
    memcpy(*front, in, BrushSize(in));
    return;
  }

  if (!(posbits & BRUSH_FRONT)) // Entirely on the back
  {
    *back = AllocBrush(in->numfaces);
    memcpy(*back, in, BrushSize(in));
    return;
  }

  brush_t* tmpfront = AllocBrush(in->numfaces * 2);
  brush_t* tmpback  = AllocBrush(in->numfaces * 2);

  for (int i = 0; i < in->numfaces; i++)
  {
    winding_t* winback = WindingEmpty();
    winding_t* winfront = WindingEmpty();
    ClipWinding(in->faces[i]->win, split, &winfront); // Check these
    ClipWinding(in->faces[i]->win, PlaneReverse(split), &winback);
    
    if (WindingValid(winback))
    {
      face_t* newface = AllocFace(in->faces[i]->plane, in->faces[i]->win->pointcount);
      free(newface->win);
      newface->win = winback;
      Brush_AddFace(&tmpfront, newface); // Add face to back
    }
    else
    {
      free(winback);
    }
    if (WindingValid(winfront))
    {
      face_t* newface = AllocFace(in->faces[i]->plane, in->faces[i]->win->pointcount);
      free(newface->win);
      newface->win = winfront;
      Brush_AddFace(&tmpback, newface); // Add face to front

    }
    else
    {
      free(winfront);
    }
  }

  // Seal by adding the split plane
  face_t* frontseal = AllocFace(PlaneReverse(split), 8);
  face_t* backseal = AllocFace(split, 8);

  Brush_AddFace(&tmpfront, frontseal);
  Brush_AddFace(&tmpback, backseal);

  if (tmpfront->numfaces >= 4)
  {
    *front = AllocBrush(tmpfront->numfaces);
    memcpy(*front, tmpfront, BrushSize(tmpfront));
  }
  if (tmpback->numfaces >= 4)
  {
    *back = AllocBrush(tmpback->numfaces);
    memcpy(*back, tmpback, BrushSize(tmpback));
  }
}

