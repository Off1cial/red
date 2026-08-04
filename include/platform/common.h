#pragma once


#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Common stuff innit

// 8 Bit colours

#define Q_COLOR_ESCAPE	'^'
#define Q_IsColorString(p)	( p && *(p) == Q_COLOR_ESCAPE && *((p)+1) && *((p)+1) != Q_COLOR_ESCAPE )

#define COLOR_BLACK		'0'
#define COLOR_RED		'1'
#define COLOR_GREEN		'2'
#define COLOR_YELLOW	'3'
#define COLOR_BLUE		'4'
#define COLOR_CYAN		'5'
#define COLOR_MAGENTA	'6'
#define COLOR_WHITE		'7'
#define ColorIndex(c)	( ( (c) - '0' ) & 7 )

#define S_COLOR_BLACK	"^0"
#define S_COLOR_RED		"^1"
#define S_COLOR_GREEN	"^2"
#define S_COLOR_YELLOW	"^3"
#define S_COLOR_BLUE	"^4"
#define S_COLOR_CYAN	"^5"
#define S_COLOR_MAGENTA	"^6"
#define S_COLOR_WHITE	"^7"

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

typedef unsigned char byte;
typedef uint16_t lbyte;

typedef char* string;



static inline void clampf(float* f, float lb, float ub)
{
  *f = fmaxf(*f, lb);
  *f = fminf(*f, ub);
}



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
