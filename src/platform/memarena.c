#include <stdio.h>
#include "platform/memarena.h"

pltMemArena* gPltMem = NULL;

pltMemArena* PlatformMemArena_Create(size_t size)
{
  pltMemArena* new = malloc(sizeof(pltMemArena));
  new->base = malloc(size);
  new->capacity = size;
  new->offset = 0;
  memset(new->base, 0, size);
  return new;
}

void PlatformMemArena_Destroy(pltMemArena* arena)
{
  if (!arena) return;

  if (arena->base) free(arena->base);
  arena->offset = arena->capacity = 0;
  free(arena);
}


void PlatformMemArena_Reset(pltMemArena* arena)
{
  arena->offset = 0;
}

typedef void* mem_t;

mem_t PlatformMemArena_Alloc(pltMemArena* arena, size_t size, size_t alignment)
{
  if (!arena) return NULL;
  if (size <= 0 || alignment <= 0) return NULL;

  uintptr_t base = (uintptr_t)(arena->base);
  uintptr_t current = (uintptr_t)(arena->base + arena->offset);

  // Round to alignment
  uintptr_t aligned = (current + (alignment -1)) & ~(alignment -1);

  size_t padding = aligned - current;
  size_t newoffset = arena->offset + size + padding;
  if (newoffset > arena->capacity)
  {
    printf("[PLATFORM][MEM]: Arena limit reached\n");
    exit(1);
  }
  arena->offset = newoffset;
  return (mem_t)(aligned);
}


