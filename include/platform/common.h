#pragma once


#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>

// Common stuff innit



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




