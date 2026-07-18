#include "corebase/shader.h"
#include "corebase/vectors.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void CBaseShader_Use(CBaseShader* shader)
{
  glUseProgram(shader->program);
}

void CBaseShader_Destroy(CBaseShader* shader)
{ 
  if (!shader) return;
  if (shader->program) glDeleteProgram(shader->program);
  shader->program = 0;
  free(shader);
}


static char* ReadFile(const char* path) {
  FILE* file = fopen(path, "rb");
  if (!file) { fprintf(stderr, "[SHADER]: failed to open %s\n", path); return NULL; }

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  rewind(file);

  char* buffer = malloc(size + 1);
  if (!buffer) { fclose(file); return NULL; }

  fread(buffer, 1, size, file);
  buffer[size] = '\0';
  fclose(file);
  return buffer;
}


static GLuint CompileShader(GLenum type, const char* src, const char* path) {
  GLuint id = glCreateShader(type);
  glShaderSource(id, 1, &src, NULL);
  glCompileShader(id);

  GLint success;
  glGetShaderiv(id, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[1024];
    glGetShaderInfoLog(id, 1024, NULL, infoLog);
    fprintf(stderr, "[Shader] %s compilation failed:\n%s\n", path, infoLog);
    glDeleteShader(id);
    return 0;
  }
  return id;
}


static int Shader_Load(CBaseShader* shader, const char* vertPath, const char* fragPath) {
  char* vertSrc = ReadFile(vertPath);
  char* fragSrc = ReadFile(fragPath);
  if (!vertSrc || !fragSrc) return 0;

  GLuint vs = CompileShader(GL_VERTEX_SHADER, vertSrc, vertPath);
  GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragSrc, fragPath);

  free(vertSrc);
  free(fragSrc);

  if (!vs || !fs) return 0;

  shader->program = glCreateProgram();
  glAttachShader(shader->program, vs);
  glAttachShader(shader->program, fs);
  glLinkProgram(shader->program);

  GLint success;
  glGetProgramiv(shader->program, GL_LINK_STATUS, &success);
  if (!success) {
      char infoLog[1024];
      glGetProgramInfoLog(shader->program, 1024, NULL, infoLog);
      fprintf(stderr, "[Shader] Linking failed:\n%s\n", infoLog);
      CBaseShader_Destroy(shader);
      return 0;
  }

  glDeleteShader(vs);
  glDeleteShader(fs);

  /*
  shader->uModelLoc = glGetUniformLocation(shader->program, "uModel");
  shader->uViewLoc = glGetUniformLocation(shader->program, "uView");
  shader->uProjLoc = glGetUniformLocation(shader->program, "uProj");
  shader->uColourLoc = glGetUniformLocation(shader->program, "uColour");
  shader->uTextureLoc = glGetUniformLocation(shader->program, "uTexture");
  shader->uUseTextureLoc = glGetUniformLocation(shader->program, "uUseTexture");
  shader->uUseVertexColLoc = glGetUniformLocation(shader->program, "uUseVertexCol");
  shader->uUseVertexColLoc_exclusive = glGetUniformLocation(shader->program, "uUseVertexCol_exclusive");
  */
  
  memset(shader->uniforms, -1, sizeof(GLint) * SH_UNIFORM_COUNT);

  shader->uniforms[SH_UNIFORM_MODEL] =
    glGetUniformLocation(shader->program, "uModel");

  shader->uniforms[SH_UNIFORM_VIEW] =
    glGetUniformLocation(shader->program, "uView");

  shader->uniforms[SH_UNIFORM_PROJECTION] = 
    glGetUniformLocation(shader->program, "uProj");

  shader->srcVertex = vertPath;
  shader->srcFrag = fragPath;
  return 1;
}


static void DisplayUniforms(CBaseShader* shader)
{
  for (int i = 0; i < SH_UNIFORM_COUNT; i++)
  {
    printf("Uniform=%d\n", shader->uniforms[i]);
  }
}

CBaseShader* CBaseShader_Create(const char* vert, const char* frag)
{
  CBaseShader* s = malloc(sizeof(CBaseShader));
  memset(s, 0, sizeof(CBaseShader));
  if (!Shader_Load(s, vert, frag))
  {
    free(s);
    return NULL;
  }
  DisplayUniforms(s);
  return s;
}





void CBaseShader_SetMat4(CBaseShader* shader, CBaseShaderLocType loctype, mat4 m)
{
  assert(shader);
  GLuint loc = shader->uniforms[loctype];
  glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)m.m);

  /*
  printf("Uploaded to loc %u:\n", loc);
  GLfloat matrix[16];
  glGetUniformfv(shader->program, loc, matrix);
  for (int i = 0; i < 16; i++)
    printf("%0.2f ", matrix[i]);
  printf("\n");
  */
}

