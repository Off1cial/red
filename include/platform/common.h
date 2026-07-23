#pragma once


#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>

// Common stuff innit


#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))

#define MAX(a,b) \
({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b; })

#define MIN(a,b) \
({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; })

#define CLAMP(v, lb, ub)  ( MAX(lb, MIN(v, ub)) )

#define STRING_MAX_LENGTH 256

typedef char byte;
typedef unsigned short int lbyte;

typedef char* string;

#ifdef true
#undef true
#endif

#ifdef false
#undef false
#endif

typedef enum {true, false} rboolean;


static inline void EXIT_ERROR(const char* msg)
{
  printf("[EXIT]: %s\n", msg);
  exit(1);
}


// Null-terminating strncpy from Quake III
static inline void Q_strncpy(char* dst, const char* src, size_t dstsize)
{
  strncpy(dst, src, dstsize - 1);
  dst[dstsize - 1] = '\0'; 
}

// Case-insensitive string compare upto n chars
int Q_stricmpn(const char* s1, const char* s2, int n);

// Case sensitive string compare to n chars
int Q_strncmp(const char* s1, const char* s2, int n);

// Case insensitive comparison of the entire string
int Q_stricmp(const char* s1, const char* s2);
