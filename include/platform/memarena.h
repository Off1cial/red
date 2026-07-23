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


// Cross platform allocation


#if defined(_WIN32)


#include <malloc.h>

#define ALIGNED_NEW(size, alignment)  _aligned_malloc((size), alignment)

#define ALIGNED_REALLOC(ptr, old_size, new_size, alignment)  _aligned_realloc((ptr), (new_size), alignment)

#define ALIGNED_FREE(ptr)  _aligned_free((ptr))

#else

static inline void* aligned_new(size_t size, size_t alignment) {
    void* ptr = NULL;
    if (posix_memalign(&ptr, alignment, size) != 0) {
        return NULL;
    }
    return ptr;
}

static inline void* aligned_realloc(void* ptr, size_t old_size, size_t new_size, size_t alignment) {
    void* new_ptr = aligned_new(new_size, alignment);
    if (!new_ptr) return NULL;

    if (ptr && old_size > 0) {
        size_t copy_size = old_size < new_size ? old_size : new_size;
        memcpy(new_ptr, ptr, copy_size);
        free(ptr);
    }

    return new_ptr;
}

#define ALIGNED_NEW(size, alignment)  aligned_new((size), (alignment))

#define ALIGNED_REALLOC(ptr, old_size, new_size)  aligned_realloc((ptr), (old_size), (new_size), (alignment))

#define ALIGNED_FREE(ptr) \
    free((ptr))

#endif

