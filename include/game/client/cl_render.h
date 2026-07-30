#pragma once
#include <glad/glad.h>
#include <stdint.h>
#include "corebase/mathlib.h"


typedef struct rendercommand_t
{
  
  uint64_t key; // Packed
  uint32_t mesh_handle;
  uint32_t material_handle;
  mat4 transform; 

} rendercommand_t;

typedef struct renderpass_t
{
  rendercommand_t* commands;
  size_t count, capacity;
} renderpass_t;

typedef enum renderpass_id_t
{
  RENDERPASS_SHADOW,
  RENDERPASS_OPAQUE,
  RENDERPASS_TRANSPARENT,
  RENDERPASS_UI,
  RENDERPASS_COUNT
  
} renderpass_id_t;

typedef struct renderer_t
{
  renderpass_t passes[RENDERPASS_COUNT];  
  // Use a UBO!!!!
  mat4 view;
  mat4 projection;
  vec3_t view_pos;
  GLuint camera_ubo;

  // Cache
  GLuint bound_shader;
  GLuint bound_vao;
  GLuint bound_textures[8];

  // Statistics
  uint32_t lastframe_drawcalls;
  uint32_t lastframe_triangles;

} CBaseRenderer_t;

extern CBaseRenderer_t* gRenderer;

uint8_t Renderer_Init( void );

void Renderer_FrameBegin( mat4 view, mat4 projection);
void Renderer_Submit(renderpass_id_t passid, uint32_t mesh, uint32_t material, mat4* transform, vec3_t world_pos);
void Renderer_FrameEnd( void );
