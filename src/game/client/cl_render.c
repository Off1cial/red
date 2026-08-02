#include "game/client/cl_render.h"

#include "engine/assetmanager.h"

CBaseRenderer_t* gRenderer = NULL;

static inline uint64_t SortKey(uint32_t shaderid, uint32_t materialid, float depth)
{
  uint32_t depthbits =  (uint32_t)(depth * 1000.0f);
  return ((uint64_t)(shaderid & 0xFFFF) << 48) |
         ((uint64_t)(materialid & 0xFFFFFF) << 24) |
         ((uint64_t)(depthbits & 0xFFFFFF));
}

static int CompareCommands(const void* a, const void* b)
{
    const rendercommand_t* ca = a;
    const rendercommand_t* cb = b;
    if (ca->key < cb->key) return -1;
    if (ca->key > cb->key) return 1;
    return 0;
}




void Renderer_Submit(renderpass_id_t passid, uint32_t mesh, uint32_t material, mat4* transform, vec3_t world_pos)
{
  renderpass_t* pass = &gRenderer->passes[passid];

  if (pass->count >= pass->capacity)
  {
    pass->capacity = pass->capacity ? pass->capacity * 2 : 256;
    pass->commands = realloc(pass->commands, sizeof(rendercommand_t) * pass->capacity);
  }

  assetMaterial_t* mat = &gAssetManager->materials.mats[material];
  float depth = VectorDistance(gRenderer->view_pos, world_pos);

  rendercommand_t* cmd = &pass->commands[pass->count++];
  cmd->key = SortKey(mat->shader.index, material, depth);
  cmd->material_handle = material;
  cmd->mesh_handle = mesh;
  Mat4Copy(*transform, cmd->transform);
}
