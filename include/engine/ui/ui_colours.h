#pragma once

#include <stdint.h>

typedef uint8_t rgb[3];
typedef uint8_t rgba[4];
// UI Colours are internally 0-1 floats, however
// When creating UI elements, the functions are expected to take 0-255 values

#define RGBAfromRGB(dest, rgb, a) \
do { \
    (dest)[0] = (rgb)[0]; \
    (dest)[1] = (rgb)[1]; \
    (dest)[2] = (rgb)[2]; \
    (dest)[3] = (a); \
} while (0)

#define RGBCopy(dest, rgb) \
do { \
    (dest)[0] = (rgb)[0]; \
    (dest)[1] = (rgb)[1]; \
    (dest)[2] = (rgb)[2]; \
} while (0)

/*
#define RGBSet(dest, r, g, b) \
do { \
    (dest)[0] = (r); \
    (dest)[1] = (g); \
    (dest)[2] = (b); \
} while (0)
*/

#define RGBACopy(dest, rgba) \
do { \
    (dest)[0] = (rgba)[0]; \
    (dest)[1] = (rgba)[1]; \
    (dest)[2] = (rgba)[2]; \
    (dest)[3] = (rgba)[3]; \
} while (0)

/*
#define RGBASet(dest, r, g, b, a) \
do { \
    (dest)[0] = (r); \
    (dest)[1] = (g); \
    (dest)[2] = (b); \
    (dest)[3] = (a); \
} while (0)
*/

static inline void RGBASet(rgba dst, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
  dst[0] = r; dst[1] = g; dst[2] = b; dst[3] = a;
}

static inline void RGBSet(rgb dst, uint8_t r, uint8_t g, uint8_t b)
{
  dst[0] = r; dst[1] = g; dst[2] = b;
}


// Colour presets

#define UI_COLOR_WHITE 0xFFFFFFFF
#define UI_COLOR_RED   0xFF0000FF
#define UI_COLOR_GREEN 0xFF00FF00
#define UI_COLOR_BLUE  0xFFFF0000


#define COL32(r,g,b,a) \
    ((uint32_t)(r)       | \
    ((uint32_t)(g) << 8) | \
    ((uint32_t)(b) << 16)| \
    ((uint32_t)(a) << 24))
