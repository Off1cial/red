#include "platform/cpu.h"

#include <cpuid.h>
#include <stdio.h>


cpufeatures_t pltCPU_GetFeatures(void)
{
    cpufeatures_t feat = {0};

    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;


    // CPUID leaf 1
    __cpuid(1, eax, ebx, ecx, edx);
  

    // EDX features
    feat.sse  = (edx & (1 << 25)) != 0;
    feat.sse2 = (edx & (1 << 26)) != 0;


    // ECX features
    feat.sse41 = (ecx & (1 << 19)) != 0;
    feat.sse42 = (ecx & (1 << 20)) != 0;
    feat.fma3  = (ecx & (1 << 12)) != 0;

    feat.avx = (ecx & (1 << 28)) != 0;



    // CPUID leaf 7
    __cpuid_count(7, 0, eax, ebx, ecx, edx);


    // EBX features
    feat.avx2 = (ebx & (1 << 5)) != 0;

    feat.avx512 = (ebx & (1 << 16)) != 0;


    return feat;
}


void pltCPU_PrintFeatures(cpufeatures_t feat)
{
  printf("SSE: %d\n", feat.sse);
  printf("SSE2: %d\n", feat.sse2);
  printf("SSE41: %d\n", feat.sse41);
  printf("SSE42: %d\n", feat.sse42);


  printf("AVX: %d\n", feat.avx);
  printf("AVX2: %d\n", feat.avx2);
  printf("AVX512: %d\n", feat.avx512);

  printf("FMA3: %d\n", feat.fma3);
}
