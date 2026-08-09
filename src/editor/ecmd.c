#include "editor/editor.h"

#include <stdio.h>
#include <stdalign.h>
#include "editor/brush.h"
#include "platform/memarena.h"


static void* ArenaAlloc(size_t size, size_t alignment)
{
  return PlatformMemArena_Alloc(gPltMem, size, alignment);
}



#define ECMD_MAX 64

typedef struct 
{
  ecmd_t cmds[ECMD_MAX];
  int head, tail;
} ecmdqueue_t;

ecmdqueue_t* gcmdqueue = NULL;

void ECMD_Init()
{
  gcmdqueue = calloc(1, sizeof(ecmdqueue_t));
  gcmdqueue->head = gcmdqueue->tail = 0;
}

void ECMD_Destroy()
{
  if (gcmdqueue)
    free(gcmdqueue);
  gcmdqueue = NULL;
}






u8 ECMD_Push(ecmd_t* cmd)
{
  int newtail = ( gcmdqueue->tail + 1 ) % ECMD_MAX;
  if (newtail == gcmdqueue->head)
  {
    printf("[EDITOR][CMD]: Queue overflow\n");
    return 0;
  }
  gcmdqueue->cmds[gcmdqueue->tail] = *cmd;
  gcmdqueue->tail = newtail;
  return 1;
}

void ECMD_BrushCreate(vec3_t min, vec3_t max)
{
  ecmd_t cmd = {0};

  cmd.type = ECMD_BRUSHCREATE;
  VectorCopy(min, cmd.brush_create.mins);
  VectorCopy(max, cmd.brush_create.maxs);
  printf("Pushed command\n");
  vec3print(min); vec3print(max);
  ECMD_Push(&cmd);
}


void Editor_BrushCreate(vec3_t mins, vec3_t maxs)
{
  brush_t* b = Brush_Create(mins, maxs);
  Brush_AddToList(b, &gBrushes);
}

void Editor_BrushDelete(brush_t* b)
{
  if (!b)
  {
    printf("[EDITOR][BRUSH]: Attempted to delete brush, cmd has no brush!\n");
    return;
  }

  Brush_RemoveFromList(b, &gBrushes);
  Brush_Delete(b);
}




void ECMD_Execute(ecmd_t* cmd)
{
  switch(cmd->type)
  {
    case ECMD_BRUSHCREATE:
      printf("ECMD_BRUSHCREATE\n");
      Editor_BrushCreate(
          cmd->brush_create.mins, 
          cmd->brush_create.maxs); 
      break;
    case ECMD_BRUSHDELETE:
      Editor_BrushDelete(cmd->brush);
      break;
    default:
      printf("ECMD_UNKNOWN\n");
      break;
  }
}

void ECMD_Flush()
{
  while (gcmdqueue->head != gcmdqueue->tail)
  {
    ecmd_t* cmd = &gcmdqueue->cmds[gcmdqueue->head];
    ECMD_Execute(cmd);
    gcmdqueue->head = (gcmdqueue->head + 1) % ECMD_MAX;
    printf("[EDITOR][CMD]: Command executed\n");
  }
}
