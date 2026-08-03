#include "engine/ui/ui.h"
#include "engine/camera.h"
#include "platform/window.h"

#define PANEL3D_STR "3D Panel"
#define PANELTOP_STR "Top (x/z)"
#define PANELFRONT_STR "Front (x/y)"
#define PANELSIDE_STR "Side (z/y)"

typedef enum paneltype_t
{
  PANEL_3D,
  PANEL_TOP,
  PANEL_FRONT,
  PANEL_SIDE,
} paneltype_t;


typedef struct panel_t
{
  // Each holds a vertex array?
  camera_t* camera;
  rectdef rect;
  u8 type;
} panel_t;

panel_t gPanels[4];
u64 PanelFlags =
  UIWindowFlag_NoCollapse |
  UIWindowFlag_NoMove |
  UIWindowFlag_NoResize;


void InitialisePanels()
{
  gPanels[0].type = PANEL_3D;
  gPanels[1].type = PANEL_TOP;
  gPanels[2].type = PANEL_SIDE;
  gPanels[3].type = PANEL_FRONT;

  
}


static void DrawPanel_3D(panel_t p);


static void DrawPanel(panel_t p);