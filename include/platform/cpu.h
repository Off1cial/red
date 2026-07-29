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


cpufeatures_t pltCPU_GetFeatures(void);
void pltCPU_PrintFeatures(cpufeatures_t feat);
