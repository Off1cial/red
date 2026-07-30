#pragma once

typedef struct
{
    int sse;
    int sse2;
    int sse41;
    int sse42;

    int avx;
    int avx2;

    int avx512;

    int fma3;

} cpufeatures_t;

extern cpufeatures_t* gPltCPUFeatures;

void pltCPU_GetFeatures(void);
void pltCPU_PrintFeatures(void);
