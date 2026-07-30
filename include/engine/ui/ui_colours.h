#pragma once


typedef float rgb[3];
typedef float rgba[4];
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

#define RGBSet(dest, r, g, b) \
do { \
    (dest)[0] = (r); \
    (dest)[1] = (g); \
    (dest)[2] = (b); \
} while (0)

#define RGBACopy(dest, rgba) \
do { \
    (dest)[0] = (rgba)[0]; \
    (dest)[1] = (rgba)[1]; \
    (dest)[2] = (rgba)[2]; \
    (dest)[3] = (rgba)[3]; \
} while (0)

#define RGBASet(dest, r, g, b, a) \
do { \
    (dest)[0] = (r); \
    (dest)[1] = (g); \
    (dest)[2] = (b); \
    (dest)[3] = (a); \
} while (0)




// Colour presets
#define UI_SETCOLRGB_WHITE(col) RGBSet(col, 255, 255, 255)

#define UI_SETCOLRGB_RED(col) RGBSet(col, 255, 0, 0)
#define UI_SETCOLRGB_GREEN(col) RGBSet(col, 0, 255, 0)
#define UI_SETCOLRGB_BLUE(col) RGBSet(col, 0, 0, 255)


#define UI_SETCOLRGBA_WHITE(col) RGBASet(col, 255, 255, 255, 255)

#define UI_SETCOLRGBA_RED(col) RGBASet(col, 255, 0, 0, 255)
#define UI_SETCOLRGBA_GREEN(col) RGBASet(col, 0, 255, 0, 255)
#define UI_SETCOLRGBA_BLUE(col) RGBASet(col, 0, 0, 255, 255)
