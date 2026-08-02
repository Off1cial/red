#pragma once
#include <stdint.h>
#include "corebase/mathlib.h"
#include "engine/mesh.h"

#define MDL_MAGIC 0x4C444D43 // "CMDL" as a uint32, little-endian
#define MDL_VERSION 1



// on-disk header — fixed size, written first
typedef struct mdlheader_t
{
  uint32_t magic;
  uint32_t version;
  uint32_t vertcount;
  uint32_t indexcount;
  char texpath[256];
} mdlheader_t;

typedef struct modelfile_t
{
  mdlheader_t header;
  gpuVertex *verts; // heap-allocated after loading, freed by caller when done
  uint32_t* indices;
} modelfile_t;

typedef struct model_t
{
  CBaseMesh *mesh;
  uint32_t texturehandle;
} model_t;

void OBJ_Load(
    const char *path, 
    gpuVertex **out, uint32_t *out_vertcount,
    uint32_t **out_indices, uint32_t *out_count);
uint8_t ModelFile_Write(const char *objsource, const char *texpath, const char *destination);
uint8_t ModelFile_Read(const char *path, modelfile_t *out);


uint8_t ModelFile_GetModel(const char* path, model_t* out, mdlheader_t* headerout);
void ModelFile_Free(modelfile_t *mdl);

extern model_t testmodel;
