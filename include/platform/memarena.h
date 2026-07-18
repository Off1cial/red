#pragma once

#include <stdint.h>
#include <stdlib.h>

typedef struct PLT_MEM_ARENA
{
  uint8_t* base;
  size_t capacity;
  size_t offset;
} pltMemArena;

pltMemArena* pltMemArena_Create(size_t size);
void PltMemArena_Destroy(pltMemArena* arena);

void PltMemArena_Reset(pltMemArena* arena);
void PltMemArena_Alloc(pltMemArena* arena, size_t size, size_t alignment);


// Client memory arena -> Go client-server eventually
#define PLT_MEM_ARENA_GSIZE ( (size_t) 32768 )
extern pltMemArena* gMemArena;