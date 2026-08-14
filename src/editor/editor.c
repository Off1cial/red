#include "editor/editor.h"
#include "editor/brush.h"
#include "engine/ui/ui.h"
#include "platform/input.h"
#include "platform/common.h"
#include <float.h>

#define SCROLL_DEADZONE 0.05f
u8 gHoveredPanelValid = 0;
u8 gBrushDrawing = 0;
brushdraw_t gBrushDraw = {0};
brush_t* gHoveredBrush = NULL;
int gHoveredBrushFace = -1;
int gHoveredBrushFacePrevious = -2;

facehighlight_t gFaceHighlight;


float gGridSizes[] =
{
  1.0f,
  10.0f,
  100.0f,
  1000.0f,
};

static inline u8 ValueOverDeadzone(float val, float zone)
{
  return (fabsf(val) >= zone);
}

static inline float SnapToGrid(float val, float grid)
{
  return roundf(val / grid) * grid;
}

static void SnapPanelPoint(panel_t* p, vec3_t point)
{
  float grid = CurrentGridSize(p->camera->fov);   // fov is your "zoom" (world units/pixel)
  point[p->axis_a] = SnapToGrid(point[p->axis_a], grid);
  point[p->axis_b] = SnapToGrid(point[p->axis_b], grid);
}

static inline bool bounds_tinyany(vec3_t mins, vec3_t maxs) {
  return fabsf(maxs[0] - mins[0]) < TINY_LIMIT ||
    fabsf(maxs[1] - mins[1]) < TINY_LIMIT ||
      fabsf(maxs[2] - mins[2]) < TINY_LIMIT;
}

static inline bool panel_hover(panel_t* p)
{
  return (gPltInput->mx >= p->rect[RECT_X] && gPltInput->mx <= p->rect[RECT_X] + p->rect[RECT_W] && 
          gPltInput->my >= p->rect[RECT_Y] && gPltInput->my <= p->rect[RECT_Y] + p->rect[RECT_H]        
      );
}

static void PanelSetHovered()
{
  for (int i = 0; i < PANEL_CONTEXT; i++)
  {
    if (panel_hover(&gPanels[i]))
    {
      gHoveredPanel = gPanels[i].type;
      return;
    }
  }
}

static void update_facehighlight() {
  face_t face = *gHoveredBrush->faces[gHoveredBrushFace];
  if (!gFaceHighlight.mesh)
  {
    gFaceHighlight.mesh = CBaseMesh_Create(face.win->pointcount, face.win->pointcount);
  }
  CBaseMesh_Reset(gFaceHighlight.mesh);
  gFaceHighlight.winding = face.win;
  u32 indices[face.win->pointcount];
  vec3_t delta;
  VectorScale(face.plane.normal, 0.1f, delta);
  for (int i = 0; i < face.win->pointcount; i++)
  {
    gpuVertex vertex;
    VectorCopy(face.win->points[i], vertex.xyz);
    VectorAdd(vertex.xyz, delta, vertex.xyz);
    VectorCopy(face.plane.normal, vertex.normal);
    //face_uvs(&face, vertex.xyz, vertex.uv);
    Vector4(vertex.col, 0, 0.9f, 0.6, 0.6f);
    indices[i] = CBaseMesh_PushVertex(gFaceHighlight.mesh, vertex);
  }
  for (int i = 1; i < face.win->pointcount - 1; i++) {
    CBaseMesh_PushTriangleIndices(gFaceHighlight.mesh, indices[0], indices[i], indices[i + 1]);
  }
  CBaseMesh_Upload(gFaceHighlight.mesh, GL_DYNAMIC_DRAW);
}

static void brush_draw(panel_t* p)
{
  if (pltInput_KeyboardPress(SDL_SCANCODE_ESCAPE))
  {
    gBrushDrawing = 0;
    return;
  }

  // Draw start
  if (pltInput_MouseClick(0) && !gBrushDrawing)
  {
    gBrushDrawing = 1;
    Panel_ScreenToWorld(p, gPltInput->mx, gPltInput->my, gBrushDraw.a);
    Panel_ScreenToWorld(p, gPltInput->mx, gPltInput->my, gBrushDraw.b);
    SnapPanelPoint(p, gBrushDraw.a);
    SnapPanelPoint(p, gBrushDraw.b);
  }

  // While Drawing
  if (gBrushDrawing && pltInput_MouseDown(0))   
  {
    Panel_ScreenToWorld(p, gPltInput->mx, gPltInput->my, gBrushDraw.b);
    SnapPanelPoint(p, gBrushDraw.b);
  }

  // Draw end
  if (pltInput_MouseRelease(0) && gBrushDrawing)
  {
    Panel_ScreenToWorld(p, gPltInput->mx, gPltInput->my, gBrushDraw.b);
    SnapPanelPoint(p, gBrushDraw.b);
    printf("Min = "); vec3print(gBrushDraw.a);
    printf("Max = "); vec3print(gBrushDraw.b);
    vec3_t mins, maxs;
    VectorMins(gBrushDraw.a, gBrushDraw.b, mins);
    VectorMaxs(gBrushDraw.a, gBrushDraw.b, maxs);

    float minval = 0;
    float maxval = BRUSH_MINSIZE * 4;

    switch(p->type)
    {
      case PANEL_TOP:
        mins[1] = minval;
        maxs[1] = maxval;
        break;
      case PANEL_FRONT:
        mins[2] = minval;
        maxs[2] = maxval;
        break;
      case PANEL_SIDE:
        mins[0] = minval;
        maxs[0] = maxval;
        break;
      default:
        break;
    }
    float d = VectorDistance(mins, maxs);
    if (bounds_tinyany(mins, maxs)) {
      gBrushDrawing = 0;
      return;

    }
    ECMD_BrushCreate(mins, maxs);
    gBrushDrawing = 0;
  }
}


// draw.c doesnt compute the brushmesh bounds yet
static inline bool inbounds2d(vec2_t mins, vec2_t maxs, const float x, const float y)
{
  return (mins[0] <= x && x <= maxs[0]) && (mins[1] <= y && y <= maxs[1]);
}

/* Idea:
 *   Using only brush face normals, decide if a point is behind the plane or not
 */
static u8 brush_guihover(brush_t* b, vec3_t worldpos)
{
  u8 hovered = 0;
  for (int i = 0; i < b->numfaces; i++)
  {
    plane_t p = b->faces[i]->plane;
    float d = DotProduct(p.normal, worldpos) - p.d;
    if ( d > EPSILON )
      return 0;
  }
  return 1;
}

/*
static inline u8 brush_contains(brush_t* b, vec3_t p)
{
  for (int i = 0; i < b->numfaces; i++)
  {
    plane_t plane = b->faces[i]->plane;
    float d = DotProduct(plane.normal, p) - plane.d;
    if (d > 0.01f)
      return 0;
  }
  printf("Point in brush, p = "); vec3print(p);
  return 1;
}
*/

static inline u8 brush_contains(brush_t* b, vec3_t p)
{

  for (int i = 0; i < b->numfaces; i++)
  {
    plane_t plane = b->faces[i]->plane;

    float dot = DotProduct(plane.normal, p);
    float d = dot - plane.d;


    if (d > 0.01f)
    {
      return 0;
    }
  }

  return 1;
}


static void brushes_guihover(panel_t* p)
{
  // Since brush geometry is always convex, we can exit early if we are infront of any one plane?
  const float mx = gPltInput->mx;
  const float my = gPltInput->my;
  brush_t* b = NULL;
  vec3_t worldpos;
  Panel_ScreenToWorld(p, mx, my, worldpos);
  for (b = gBrushes; b ; b = b->next)
  { 
    vec2_t mins, maxs;
    switch(p->type)
    {
      case PANEL_SIDE:
        Vector2Copy(b->mesh.sidebounds.mins, mins);
        Vector2Copy(b->mesh.sidebounds.maxs, maxs);
        break;
      case PANEL_FRONT:
        Vector2Copy(b->mesh.frontbounds.mins, mins);
        Vector2Copy(b->mesh.frontbounds.maxs, maxs);
        break;
      case PANEL_TOP:
        Vector2Copy(b->mesh.topbounds.mins, mins);
        Vector2Copy(b->mesh.topbounds.maxs, maxs);
        break;
      default:
        return;
    }
    // Add AABB checking?
    if (!inbounds2d(mins, maxs, mx, my))
      continue;

    if (brush_guihover(b, worldpos))
    {
      gHoveredBrush = b;
      printf("HOVER\n");
      return;
    }
  }
}

static bool brush_ray()
{
  vec3_t raydir, rayorigin;
  Camera_Screenray(gCamera, gPltInput->mx, gPltInput->my, rayorigin, raydir);
  //printf("Ray "); vec3print(raydir); 
  float t_closest = FLT_MAX;
  brush_t* b_closest = NULL;
  vec3_t hit_closest;
  int hovered_face = -1;

  brush_t* b;
  for (b = gBrushes; b; b = b->next)
  {
    for (int i = 0; i < b->numfaces; i++)
    {
      face_t* f = b->faces[i];
      float t;
      vec3_t hit;
      if (!PlaneLineIntersection(rayorigin, raydir, f->plane, hit, &t))
        continue;

   //printf("origin: "); vec3print(rayorigin);
  //printf("dir: "); vec3print(raydir);

//printf("face %d: t=%f hit=", i, t);
//vec3print(hit);


      if (t >= t_closest || t < 0.0f)
        continue;
    
      if (!brush_contains(b, hit))
        continue;
       

      t_closest = t;
      VectorCopy(hit, hit_closest);
      b_closest = b;
      hovered_face = i;
      
    }
  }
  gHoveredBrush = b_closest;
  gHoveredBrushFace = hovered_face;
  return (gHoveredBrush != NULL);
}

static void ortho_input(panel_t* p)
{
  camera_t* cam = p->camera;
  if (!cam) return;
  float* camx = &cam->origin[p->axis_a];
  float* camy = &cam->origin[p->axis_b];
  float movscale = fmaxf(cam->fov, 0.1f);
  
  if (ValueOverDeadzone(gPltInput->mscrl_x, SCROLL_DEADZONE))
    *camx = (*camx) + (gPltInput->mscrl_x * movscale);
  if (ValueOverDeadzone(gPltInput->mscrl_y, SCROLL_DEADZONE))
    *camy = (*camy) + (gPltInput->mscrl_y * movscale);

  if (pltInput_KeyboardPress(SDL_SCANCODE_RIGHTBRACKET))
    cam->fov *= 0.9f;

  if (pltInput_KeyboardPress(SDL_SCANCODE_LEFTBRACKET))
    cam->fov *= 1.1f;
  clampf(&cam->fov, 0.01f, 2.0f);


  // Brush Drawing

  if (gHoveredPanel == p->type)
  {
    brush_draw(p);
    brushes_guihover(p);
  }

}

// 3D panel
static void view_input() {
  if (!brush_ray())
    return;

  if (gHoveredBrushFace != gHoveredBrushFacePrevious)
  {
    gHoveredBrushFacePrevious = gHoveredBrushFace;
    update_facehighlight();
  }
}

void PanelInput()
{
  PanelSetHovered();
  if (gHoveredPanel < 0)
    return;


  panel_t* p = &gPanels[gHoveredPanel];
  if (p->type == PANEL_TOP  || p->type == PANEL_FRONT || p->type == PANEL_SIDE)
    ortho_input(p);

  if (gHoveredPanel == PANEL_3D)
  {
    /*
    if (brush_ray()) {
      if (gHoveredBrushFace != gHoveredBrushFacePrevious) {
        gHoveredBrushFacePrevious = gHoveredBrushFace;
        update_facehighlight();
      }
    }
    */
    view_input();
  }

}
