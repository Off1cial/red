#include "platform/cpu.h"

#include <cpuid.h>
#include <stdio.h>
#include <string.h>

cpufeatures_t *gPltCPUFeatures;

void pltCPU_GetFeatures(void)
{
  memset(gPltCPUFeatures, 0, sizeof(cpufeatures_t));

  unsigned int eax;
  unsigned int ebx;
  unsigned int ecx;
  unsigned int edx;

  // CPUID leaf 1
  __cpuid(1, eax, ebx, ecx, edx);

  // EDX features
  gPltCPUFeatures->sse = (edx & (1 << 25)) != 0;
  gPltCPUFeatures->sse2 = (edx & (1 << 26)) != 0;

  // ECX features
  gPltCPUFeatures->sse41 = (ecx & (1 << 19)) != 0;
  gPltCPUFeatures->sse42 = (ecx & (1 << 20)) != 0;
  gPltCPUFeatures->fma3 = (ecx & (1 << 12)) != 0;

  gPltCPUFeatures->avx = (ecx & (1 << 28)) != 0;

  // CPUID leaf 7
  __cpuid_count(7, 0, eax, ebx, ecx, edx);

  // EBX features
  gPltCPUFeatures->avx2 = (ebx & (1 << 5)) != 0;

  gPltCPUFeatures->avx512 = (ebx & (1 << 16)) != 0;
}

void pltCPU_PrintFeatures(void)
{
  printf("SSE: %d\n", gPltCPUFeatures->sse);
  printf("SSE2: %d\n", gPltCPUFeatures->sse2);
  printf("SSE41: %d\n", gPltCPUFeatures->sse41);
  printf("SSE42: %d\n", gPltCPUFeatures->sse42);

  printf("AVX: %d\n", gPltCPUFeatures->avx);
  printf("AVX2: %d\n", gPltCPUFeatures->avx2);
  printf("AVX512: %d\n", gPltCPUFeatures->avx512);

  printf("FMA3: %d\n", gPltCPUFeatures->fma3);
}
