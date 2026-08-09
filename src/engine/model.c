#include <stdio.h>
#include <stdlib.h>
#include "engine/model.h"

typedef struct
{
  vec3_t *data;
  int count;
  int cap;
} vec3arr_t;
typedef struct
{
  vec2_t *data;
  int count;
  int cap;
} vec2arr_t;

static void PushVec3(vec3arr_t *arr, vec3_t v)
{
  if (arr->count >= arr->cap)
  {
    arr->cap = arr->cap ? arr->cap * 2 : 64;
    arr->data = realloc(arr->data, sizeof(vec3_t) * arr->cap);
  }
  int i = arr->count;
  arr->data[i][0] = v[0];
  arr->data[i][1] = v[1];
  arr->data[i][2] = v[2];
  arr->count++;
}

static void PushVec2(vec2arr_t *arr, vec2_t v)
{
  if (arr->count >= arr->cap)
  {
    arr->cap = arr->cap ? arr->cap * 2 : 64;
    arr->data = realloc(arr->data, sizeof(vec2_t) * arr->cap);
  }
  int i = arr->count;
  arr->data[i][0] = v[0];
  arr->data[i][1] = v[1];
  arr->count++;
}


#define OBJ_HASH_BUCKETS 4096

typedef struct 
{
  int vi, ti, ni;
  uint32_t index;
} objkeymap_entry_t; 

typedef struct
{
  gpuVertex* verts;
  uint32_t vertcount, vertcap;

  uint32_t* indices;
  uint32_t indexcount, indexcap;

  objkeymap_entry_t* buckets[OBJ_HASH_BUCKETS];
  int bucketcount[OBJ_HASH_BUCKETS];
  int bucketcap[OBJ_HASH_BUCKETS];

} meshbuild_t;

static uint32_t HashObjKey(int vi, int ti, int ni)
{
  uint32_t h = 2166136261u;
  h ^= (uint32_t)vi; h*= 16777619u;
  h ^= (uint32_t)ti; h*= 16777619u;
  h ^= (uint32_t)ni; h*= 16777619u;
  return h % OBJ_HASH_BUCKETS;
}

static void MeshBuild_PushIndex(meshbuild_t* mb, uint32_t idx)
{
  if (mb->indexcount >= mb->indexcap)
  {
    mb->indexcap = mb->indexcap ? mb->indexcap * 2 : 64;
    mb->indices = realloc(mb->indices, sizeof(uint32_t) * mb->indexcap);
  }
  mb->indices[mb->indexcount++] = idx;
}

static void MeshBuild_PushVertex(meshbuild_t* mb, gpuVertex v)
{
  if (mb->vertcount >= mb->vertcap)
  {
    mb->vertcap = mb->vertcap ? mb->vertcap * 2 : 64;
    mb->verts = realloc(mb->verts, sizeof(gpuVertex) * mb->vertcap);
  }
  mb->verts[mb->vertcount++] = v;
}

static uint32_t MeshBuild_GetAddVertex(meshbuild_t* mb, int vi, int ti, int ni, gpuVertex v)
{
  uint32_t bucket = HashObjKey(vi, ti, ni);

  for (int i = 0; i < mb->bucketcount[bucket]; i++)
  {
    objkeymap_entry_t* e = &mb->buckets[bucket][i];
    if (e->vi == vi && e->ti == ti && e->ni == ni)
      return e->index;
  }

  uint32_t newindex = mb->vertcount;
  MeshBuild_PushVertex(mb, v);

  if (mb->bucketcount[bucket] >= mb->bucketcap[bucket])
  {
    // Expand bucket
    mb->bucketcap[bucket] = 
      mb->bucketcap[bucket] ? 
      mb->bucketcap[bucket] * 2 : 
      4;
    
    mb->buckets[bucket] = 
      realloc(
          mb->buckets[bucket], 
          sizeof(objkeymap_entry_t) * mb->bucketcap[bucket]);
  }
  mb->buckets[bucket][mb->bucketcount[bucket]] = (objkeymap_entry_t){vi, ti, ni, newindex};
  mb->bucketcount[bucket]++;

  return newindex;
}


static void MeshBuild_Free(meshbuild_t* mb)
{
  for (int i = 0; i < OBJ_HASH_BUCKETS; i++)
  {
    if (mb->buckets[i]) free(mb->buckets[i]);
    mb->buckets[i] = NULL;
  }
}




static inline void ResolveAssetPath(const char *path_in_assets, char *out, size_t outsize)
{
  snprintf(out, outsize, "%s/%s", ASSET_DIR, path_in_assets);
}


// ASSUME CONVEX FACES
void OBJ_Load(
    const char *path, 
    gpuVertex **out_verts, uint32_t *out_vertcount,
    uint32_t  **out_indices, uint32_t* out_indexcount)
{
  char abspath[256];
  ResolveAssetPath(path, abspath, 256);
  FILE *objfile = fopen(abspath, "r");

  if (!objfile)
  {
    printf("[OBJ]: Failed to load OBJ file, %s\n", path);
    *out_vertcount = 0;
    *out_indexcount = 0;
    return;
  }

  vec3arr_t positions = {0};
  vec3arr_t normals = {0};
  vec2arr_t uvs = {0};

  meshbuild_t mb = {0};


  char line[256];
  while (fgets(line, sizeof(line), objfile))
  {
    if (line[0] == 'v' && line[1] == ' ')
    {
      vec3_t v; // vertex position
      sscanf(line, "v %f %f %f", &v[0], &v[1], &v[2]);
      PushVec3(&positions, v);
    }
    else if (line[0] == 'v' && line[1] == 'n')
    {
      vec3_t vn; // vertex normal
      sscanf(line, "vn %f %f %f", &vn[0], &vn[1], &vn[2]);
      PushVec3(&normals, vn);
    }
    else if (line[0] == 'v' && line[1] == 't')
    {
      vec2_t vt; // vertex texture
      sscanf(line, "vt %f %f", &vt[0], &vt[1]);
      PushVec2(&uvs, vt);
    }
    else if (line[0] == 'f'&& line[1] == ' ')
    {
      // Fakin meaty triangulation, kant.
      int vi[32], ti[32], ni[32];
      int n = 0;

      char* cursor = line + 2;
      while (n < 32 && sscanf(cursor, "%d/%d/%d", &vi[n], &ti[n], &ni[n]) == 3)
      {
        n++;
        while (*cursor && *cursor != ' ' && *cursor != '\n') cursor++;
        while (*cursor == ' ') cursor++;
        if (*cursor == '\n' || *cursor == '\0') break;
      }

      if (n < 3)
      {
        printf("[OBJ]: Face with fewer than 3 vertices in %s\n", path);
        continue;
      }

      // Fan triangulate
      int tricount = n - 2;
      for (int t = 0; t < tricount; t++)
      {
        int idx[3] = {0, t + 1, t + 2};
        
        for (int k = 0; k < 3; k++)
        {
          int fi = idx[k];

          gpuVertex gv = {0};
          VectorCopy(positions.data[ vi[fi] - 1 ], gv.xyz);
          VectorCopy(normals.data[ ni[fi] - 1 ], gv.normal);
          gv.uv[0] = uvs.data[ ti[fi] - 1 ][0];
          gv.uv[1] = uvs.data[ ti[fi] - 1 ][1];
          Vector(gv.col, 0.6f, 0.6f, 0.6f);

          uint32_t meshindex = MeshBuild_GetAddVertex(&mb, vi[fi], ti[fi], ni[fi], gv);
          MeshBuild_PushIndex(&mb, meshindex);
        }
      }
    }
  }
  fclose(objfile);
  free(positions.data);
  free(normals.data);
  free(uvs.data);

  *out_verts = mb.verts;
  *out_vertcount = mb.vertcount;

  *out_indices = mb.indices;
  *out_indexcount = mb.indexcount;

  MeshBuild_Free(&mb);

}

uint8_t ModelFile_Write(const char *objsource, const char *texpath, const char *destination)
{
  gpuVertex *verts;
  uint32_t *indices;
  uint32_t vertcount;
  uint32_t indexcount;

  

  OBJ_Load(objsource, &verts, &vertcount, &indices, &indexcount);
  if (vertcount == 0 || indexcount == 0)
  {
    printf("[MDL]: No vertices loaded from %s\n", objsource);
    if (verts) free(verts);
    if (indices) free(indices);
    return 0;
  }

  char abstextpath[256], abspath[256];
  ResolveAssetPath(destination, abspath, 256);
  ResolveAssetPath(texpath, abstextpath, 256);
  FILE *dest = fopen(abspath, "wb");
  if (!dest)
  {
    printf("[MDL]: Failed to open %s for writing\n", abspath);
    if (verts) free(verts);
    if (indices) free(indices);
    return 0;
  }

  mdlheader_t header = {0};
  header.magic = MDL_MAGIC;
  header.version = MDL_VERSION;
  header.vertcount = vertcount;
  header.indexcount = indexcount;
  strncpy(header.texpath, texpath, sizeof(header.texpath) - 1);

  size_t written = 0;
  written += fwrite(&header, sizeof(mdlheader_t), 1, dest);
  written += fwrite(verts, sizeof(gpuVertex), vertcount, dest);
  written += fwrite(indices, sizeof(uint32_t), indexcount, dest);


  fclose(dest);
  if (verts)   free(verts);
  if (indices) free(indices);

  if (written != 1 + vertcount + indexcount)
  {
    printf("[MDL]: Incomplete write to %s\n", destination);
    return 0;
  }

  printf("[MDL]: Wrote %s (%u verts)\n", destination, vertcount);
  return 1;
}

uint8_t ModelFile_Read(const char *path, modelfile_t *out)
{
  char abspath[256];
  ResolveAssetPath(path, abspath, 256);
  FILE *f = fopen(abspath, "rb");
  if (!f)
  {
    printf("[MDL]: Failed to open %s\n", path);
    return 0;
  }

  if (fread(&out->header, sizeof(mdlheader_t), 1, f) != 1)
  {
    printf("[MDL]: Failed to read header from %s\n", path);
    fclose(f);
    return 0;
  }

  if (out->header.magic != MDL_MAGIC)
  {
    printf("[MDL]: Bad magic in %s (not a valid .mdl file)\n", path);
    fclose(f);
    return 0;
  }
  if (out->header.version != MDL_VERSION)
  {
    printf("[MDL]: Unsupported version %u in %s\n", out->header.version, path);
    fclose(f);
    return 0;
  }

  out->verts = malloc(sizeof(gpuVertex) * out->header.vertcount);
  out->indices = malloc(sizeof(uint32_t) * out->header.indexcount);
  if (!out->verts || !out->indices)
  {
    printf("[MDL]: Allocation failed for %s\n", path);
    if (out->verts)   free(out->verts);
    if (out->indices) free(out->indices);
    fclose(f);
    return 0;
  }

  if (fread(out->verts, sizeof(gpuVertex), out->header.vertcount, f) != out->header.vertcount)
  {
    printf("[MDL]: Truncated vertex data in %s\n", path);
    free(out->verts);
    fclose(f);
    return 0;
  }

  if (fread(out->indices, sizeof(uint32_t), out->header.indexcount, f) != out->header.indexcount)
  {
    printf("[MDL]: Truncated index data in %s\n", path);
    if (out->verts) free(out->verts);
    if (out->indices) free(out->indices);
    fclose(f);
    return 0;
  }

  fclose(f);
  return 1;
}

void ModelFile_Free(modelfile_t *mdl)
{
  if (mdl->verts) free(mdl->verts);
  if (mdl->indices) free(mdl->indices);
  
  mdl->verts = NULL;
  mdl->indices = NULL;
}


model_t testmodel;

uint8_t ModelFile_GetModel(const char* path, model_t* out, mdlheader_t* headerout)
{
  if (!out || !path)
    return 0;

  modelfile_t file;
  if (!ModelFile_Read(path, &file))
    return 0;
  
  int vertexcount = (int)file.header.vertcount;
  int indexcount = (int)file.header.indexcount;

  CBaseMesh* mesh = CBaseMesh_Create(vertexcount, indexcount);

  for (uint32_t i = 0; i < vertexcount; i++)
    CBaseMesh_PushVertex(mesh, file.verts[i]);

  for (uint32_t i = 0; i + 2 < indexcount; i += 3)
  {
    CBaseMesh_PushTriangleIndices(
        mesh,
        file.indices[i],
        file.indices[i + 1],
        file.indices[i + 2]
        );
  }

  CBaseMesh_Upload(mesh, GL_STATIC_DRAW);
  out->mesh = mesh;

  *headerout = file.header;
  ModelFile_Free(&file);

  return 1;
}
