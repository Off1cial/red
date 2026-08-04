#include "engine/assetmanager.h"
#include "engine/ui/ui_draw.h"
#include <stdlib.h>
#include "platform/common.h"
#include "platform/window.h"
#include "engine/shader.h"

#define UI32_INVALID ((uint32_t)-1) // 0xFFFFFFFF

static inline u32 RGBA_32(rgba col)
{
  return COL32(col[0], col[1], col[2], col[3]);
}

static uint8_t grow_vertex_array()
{
  size_t newcap = (gUIctx->vertexcapacity) * 2;
  size_t c = sizeof(uivertex_t) * newcap;
  uivertex_t* vertices = realloc(
      gUIctx->vertices,
      c);
  
  if (!vertices)
    return 0;
  
  gUIctx->vertices = vertices;
  gUIctx->vertexcapacity = newcap;

  return 1;
}

static uint8_t grow_index_array()
{
  size_t newcap = gUIctx->indexcapacity * 2;
  size_t c = sizeof(uint32_t) * newcap;
  uint32_t* indices = realloc(
      gUIctx->indices,
      c
      );

  if (!indices)
    return 0;

  gUIctx->indices = indices;
  gUIctx->indexcapacity = newcap;

  return 1;
}

static uint8_t TextBatch_Init(ui_textbatch_t* dest, uint32_t font)
{
  if (!dest)
    return 0;

  // Check for existing batch of this font
  for (int i = 0; i < gUIctx->textbatch_count; i++)
  {
    if (gUIctx->textbatches[i].font == font)
      return 1;
  }

  size_t initial_cap = 512;
  dest->vertices = malloc(sizeof(uivertex_t) * initial_cap);
  dest->indices = malloc(sizeof(uint32_t) * initial_cap);

  if (!dest->vertices || !dest->indices)
    return 0;

  dest->vertexcount = 0;
  dest->indexcount = 0;
  dest->vertexcapacity = initial_cap;
  dest->indexcapacity = initial_cap;
  dest->font = font;

  glGenVertexArrays(1, &dest->vao);
  glGenBuffers(1, &dest->vbo);
  glGenBuffers(1, &dest->ebo);

  return 1;
}

static uint32_t TextBatch_PushVertex(ui_textbatch_t* batch, uivertex_t v)
{
  if (!batch)
  {
    EXIT_ERROR("[UI][TEXT]: Batch is null");
  }
  if (batch->vertexcount >= batch->vertexcapacity)
  {
    batch->vertexcapacity *= 2;
    batch->vertices = realloc(batch->vertices, sizeof(uivertex_t) * batch->vertexcapacity);
  }
  batch->vertices[batch->vertexcount] = v;
  return batch->vertexcount++;
}

static void TextBatch_PushTriangle(ui_textbatch_t* batch, uint32_t i0, uint32_t i1, uint32_t i2)
{
  if (batch->indexcount + 3 >= batch->indexcapacity)
  {
    batch->indexcapacity *= 2;
    batch->indices = realloc(batch->indices, sizeof(uint32_t) * batch->indexcapacity);
  }
  batch->indices[batch->indexcount++] = i0;
  batch->indices[batch->indexcount++] = i1;
  batch->indices[batch->indexcount++] = i2;
}

static uint32_t UI_PushVertex(uivertex_t v)
{
  if (!gUIctx->vertices)
    return UI32_INVALID;
  
  if (gUIctx->vertexcount >= gUIctx->vertexcapacity)
  {
    if (!grow_vertex_array())
      return UI32_INVALID;
  }

  gUIctx->vertices[gUIctx->vertexcount] = v;
  return gUIctx->vertexcount++;
}

static uint8_t UI_PushTriangle(
    uint32_t i0, 
    uint32_t i1,
    uint32_t i2
)
{
  if (i0 == UI32_INVALID ||
      i1 == UI32_INVALID ||
      i2 == UI32_INVALID)
    return 0;


  if (gUIctx->indexcount + 3 >= gUIctx->indexcapacity)
  {
    if (!grow_index_array())
      return 0;
  }

  gUIctx->indices[gUIctx->indexcount++] = i0; 
  gUIctx->indices[gUIctx->indexcount++] = i1; 
  gUIctx->indices[gUIctx->indexcount++] = i2; 
  return 1;
}


static void UI_ScreenToNDC(float x, float y, float out[2])
{
    out[0] = (x / gPltWindow->winw) * 2.0f - 1.0f;
    out[1] = 1.0f - (y / gPltWindow->winh) * 2.0f;
}

static inline void uivertex_setpos(uivertex_t* v, float x, float y)
{
  //UI_ScreenToNDC(pos[0], pos[1], v->pos);
  v->pos[0] = x;
  v->pos[1] = y;
}

static inline void uivertex_setcolour(uivertex_t* v, rgba colour)
{
  uint32_t col = COL32(colour[0], colour[1], colour[2], colour[3]);
  v->col = col;
}

static inline void UI_AddChar(const char c, uint32_t fontid, float posx, float posy, rgba colour)
{
  // Find the text batch for this font
  ui_textbatch_t* batch = NULL;
  for (int i = 0; i < gUIctx->textbatch_count; i++)
  {
    if (fontid == gUIctx->textbatches[i].font)
    {
      batch = &gUIctx->textbatches[i];
      break;
    }
  }
  if (!batch)
  {
    // Create a batch
    TextBatch_Init(&gUIctx->textbatches[gUIctx->textbatch_count], fontid);
    batch = &gUIctx->textbatches[gUIctx->textbatch_count++];
  }
  assetFont_t* font = AssetManager_GetFont(fontid);
  byte idx = (byte)c;
  if (idx < 32 || idx >= ASSETS_MAX_FONT_GLYPHS)
    return;

  struct _fontglyph* glyph = &font->glyphs[idx];
  if (glyph->w == 0 && glyph->h == 0)
    return;
  
  //float base = posy + font->ascent;
  float x1 = posx + glyph->w;
  float y1 = posy + glyph->h;

  uint8_t ascii = (uint8_t)(c);

  uivertex_t v0, v1, v2, v3;
  uivertex_setcolour(&v0, colour);
  uivertex_setcolour(&v1, colour);
  uivertex_setcolour(&v2, colour);
  uivertex_setcolour(&v3, colour);
  
  //UI_ScreenToNDC(posx, posy, v0.pos);
  //UI_ScreenToNDC(x1, posy, v1.pos);
  //UI_ScreenToNDC(x1, y1, v2.pos);
  //UI_ScreenToNDC(posx, y1, v3.pos);
  uivertex_setpos(&v0, posx, posy);
  uivertex_setpos(&v1, x1, posy);
  uivertex_setpos(&v2, x1, y1);
  uivertex_setpos(&v3, posx, y1);

  v0.uv[0] = glyph->uv0[0];
  v0.uv[1] = glyph->uv0[1];
  
  v1.uv[0] = glyph->uv1[0];
  v1.uv[1] = glyph->uv0[1];

  v2.uv[0] = glyph->uv1[0];
  v2.uv[1] = glyph->uv1[1];

  v3.uv[0] = glyph->uv0[0];
  v3.uv[1] = glyph->uv1[1];
  
  uint32_t i0 = TextBatch_PushVertex(batch, v0);
  uint32_t i1 = TextBatch_PushVertex(batch, v1);
  uint32_t i2 = TextBatch_PushVertex(batch, v2);
  uint32_t i3 = TextBatch_PushVertex(batch, v3);

  TextBatch_PushTriangle(batch, i0, i1, i2);
  TextBatch_PushTriangle(batch, i0, i2, i3);

}

void UI_AddText(const char* text, uint32_t fontid, float posx, float posy, rgba colour)
{
  // Find the batch for this font
  ui_textbatch_t* batch = NULL;
  for (int i = 0; i < gUIctx->textbatch_count; i++)
  {
    if (fontid == gUIctx->textbatches[i].font)
    {
      batch = &gUIctx->textbatches[i];
      break;
    }
  }
  if (!batch)
  {
    // Create a batch...
    TextBatch_Init(&gUIctx->textbatches[gUIctx->textbatch_count], fontid);
    batch = &gUIctx->textbatches[gUIctx->textbatch_count++];
  }
  float cursorx = posx;
  float cursory = posy;

  assetFont_t* font = &gAssetManager->fonts.fonts[fontid];
  for (const char* c = text; *c; c++)
  {
    byte idx = (byte)*c;
    if (idx < 32 || idx >= ASSETS_MAX_FONT_GLYPHS)
    {
      cursorx += font->glyphs[' '].advance;
      continue;
    }
    struct _fontglyph* glyph = &font->glyphs[idx];
    if (glyph->w == 0 && glyph->h == 0)
    {
      cursorx += font->glyphs[' '].advance;
      continue;
    }

    float baseline = cursory + font->ascent;
    float x0 = cursorx;
    float y0 = cursory;

    float x1 = x0 + glyph->w;
    float y1 = y0 + glyph->h;

    uint8_t ascii = (uint8_t)(*c);



    uivertex_t v0, v1, v2, v3;
    uivertex_setcolour(&v0, colour);
    uivertex_setcolour(&v1, colour);
    uivertex_setcolour(&v2, colour);
    uivertex_setcolour(&v3, colour);
    
    //UI_ScreenToNDC(x0, y0, v0.pos);
    //UI_ScreenToNDC(x1, y0, v1.pos);
    //UI_ScreenToNDC(x1, y1, v2.pos);
    //UI_ScreenToNDC(x0, y1, v3.pos);

    uivertex_setpos(&v0, x0, y0);
    uivertex_setpos(&v1, x1, y0);
    uivertex_setpos(&v2, x1, y1);
    uivertex_setpos(&v3, x0, y1);

    v0.uv[0] = glyph->uv0[0];
    v0.uv[1] = glyph->uv0[1];
    
    v1.uv[0] = glyph->uv1[0];
    v1.uv[1] = glyph->uv0[1];

    v2.uv[0] = glyph->uv1[0];
    v2.uv[1] = glyph->uv1[1];

    v3.uv[0] = glyph->uv0[0];
    v3.uv[1] = glyph->uv1[1];
    
    uint32_t i0 = TextBatch_PushVertex(batch, v0);
    uint32_t i1 = TextBatch_PushVertex(batch, v1);
    uint32_t i2 = TextBatch_PushVertex(batch, v2);
    uint32_t i3 = TextBatch_PushVertex(batch, v3);

    TextBatch_PushTriangle(batch, i0, i1, i2);
    TextBatch_PushTriangle(batch, i0, i2, i3);

    cursorx += (float)glyph->advance;
  }
}

void UI_DrawLine(vec2_t a, vec2_t b, u32 col, float thickness)
{
  vec2_t dir, perp;
  Vector2Sub(b, a, dir);
  perp[0] = -dir[1];
  perp[1] = dir[0];
  float invperpmag = 1.0f / sqrtf(perp[0] * perp[0] + perp[1] * perp[1]);
  perp[0] *= invperpmag;
  perp[1] *= invperpmag;

  // A and B have two points
  vec2_t a0, a1;
  vec2_t b0, b1;

  float halfthick = 0.5 * thickness;
  a0[0] = a[0] + perp[0] * halfthick;
  a0[1] = a[1] + perp[1] * halfthick;
  
  a1[0] = a[0] - perp[0] * halfthick;
  a1[1] = a[1] - perp[1] * halfthick;

  b0[0] = b[0] + perp[0] * halfthick;
  b0[1] = b[1] + perp[1] * halfthick;
  
  b1[0] = b[0] - perp[0] * halfthick;
  b1[1] = b[1] - perp[1] * halfthick;

  UI_DrawTriangle(a0, b0, b1, col);
  UI_DrawTriangle(b1, a1, a0, col);
}

static void uidebug_printvert(float pos[2])
{
  printf("UIvert (%0.2f, %0.2f)\n", pos[0], pos[1]);
}

static void uidebug_printcolour(float col[4])
{
  printf("Col: %0.2f, %0.2f, %0.2f, %0.2f\n", col[0], col[1], col[2], col[3]);
}





static void rect_corner(rectdef rect, int i, float out[2])
{
  switch(i)
  {
    case 0:
      out[0] = rect[RECT_X];
      out[1] = rect[RECT_Y];
      break;
    case 1:
      out[0] = rect[RECT_X];
      out[1] = rect[RECT_Y] + rect[RECT_H];
      break;
    case 2:
      out[0] = rect[RECT_X] + rect[RECT_W];
      out[1] = rect[RECT_Y] + rect[RECT_H];
      break;
    case 3:
      out[0] = rect[RECT_X] + rect[RECT_W];
      out[1] = rect[RECT_Y];
      break;
    default:
      break;
  }
}

static void rect_corner_all(rectdef rect, float out[4][2])
{
  for (int i = 0; i < 4; i++)
    rect_corner(rect, i, out[i]);
}


/*
 Now supports textures and UVs
 u1,v1 represent rect top left
 u2, v2 represent rect bottom right
 texid = -1 for no texture
*/
void UI_DrawRect(
    rectdef rect, 
    float u1, float v1,
    float u2, float v2,
    u32 col,
    GLint texid)
{
  // Upload vertices to the context batch
  if (texid != gUIctx->activetex)
  {
    // Flush rendering 
    UI_DrawBatch();
    gUIctx->activetex = texid;
  }

  uivertex_t rectv[4] = {0};
  // Do UVs in the future
  rect_corner(rect, UI_RECTCORNER_TL, rectv[UI_RECTCORNER_TL].pos);
  rect_corner(rect, UI_RECTCORNER_BL, rectv[UI_RECTCORNER_BL].pos);
  rect_corner(rect, UI_RECTCORNER_BR, rectv[UI_RECTCORNER_BR].pos);
  rect_corner(rect, UI_RECTCORNER_TR, rectv[UI_RECTCORNER_TR].pos);


  rectv[UI_RECTCORNER_TL].uv[0] = u1;
  rectv[UI_RECTCORNER_TL].uv[1] = v1;

  rectv[UI_RECTCORNER_BL].uv[0] = u1;
  rectv[UI_RECTCORNER_BL].uv[1] = v2;

  rectv[UI_RECTCORNER_BR].uv[0] = u2;
  rectv[UI_RECTCORNER_BR].uv[1] = v2;

  rectv[UI_RECTCORNER_TR].uv[0] = u2;
  rectv[UI_RECTCORNER_TR].uv[1] = v1;
  
  
  rectv[0].col = col;
  rectv[1].col = col;
  rectv[2].col = col;
  rectv[3].col = col;


  /*
  for (int i = 0; i < 4; i++)
  {
    // Move this to the GPU -> uniform vec2 screensize
    UI_ScreenToNDC(rectv[i].pos[0], rectv[i].pos[1], rectv[i].pos);
    //uidebug_printvert(rectv[i].pos);
    //uidebug_printcolour(rectv[i].colour);

  }
  */

  uint32_t i0 = UI_PushVertex(rectv[0]);
  uint32_t i1 = UI_PushVertex(rectv[1]);
  uint32_t i2 = UI_PushVertex(rectv[2]);
  uint32_t i3 = UI_PushVertex(rectv[3]);

  UI_PushTriangle(i0, i1, i2);
  UI_PushTriangle(i0, i2, i3);
}

void UI_DrawRectOutline(rectdef rect, rgba col, float thickness)
{
    rectdef top = {
        rect[RECT_X],
        rect[RECT_Y],
        rect[RECT_W],
        thickness
    };

    rectdef bottom = {
        rect[RECT_X],
        rect[RECT_Y] + rect[RECT_H] - thickness,
        rect[RECT_W],
        thickness
    };

    rectdef left = {
        rect[RECT_X],
        rect[RECT_Y],
        thickness,
        rect[RECT_H]
    };

    rectdef right = {
        rect[RECT_X] + rect[RECT_W] - thickness,
        rect[RECT_Y],
        thickness,
        rect[RECT_H]
    };

    u32 col32 = RGBA_32(col);
    UI_DrawRect(top,    0, 0, 1, 1, col32, -1);
    UI_DrawRect(bottom, 0, 0, 1, 1, col32, -1);
    UI_DrawRect(left,   0, 0, 1, 1, col32, -1);
    UI_DrawRect(right,  0, 0, 1, 1, col32, -1);
}


void UI_DrawTriangle(float v0[2], float v1[2], float v2[2], u32 col)
{
  uivertex_t verts[3];
  memset(verts, 0, sizeof(uivertex_t)* 3);

  uivertex_setpos(&verts[0], v0[0], v0[1]);
  uivertex_setpos(&verts[1], v1[0], v1[1]);
  uivertex_setpos(&verts[2], v2[0], v2[1]);
  

  verts[0].col = col;
  verts[1].col = col;
  verts[2].col = col;

  GLuint i0 = UI_PushVertex(verts[0]);
  GLuint i1 = UI_PushVertex(verts[1]);
  GLuint i2 = UI_PushVertex(verts[2]);

  UI_PushTriangle(i0, i1, i2);
}


static void UI_DrawTextBatch(ui_textbatch_t* batch)
{

  assetFont_t* font = &gAssetManager->fonts.fonts[batch->font];

  if (font->tex_index != -1)
  {
    GLuint texid = gAssetManager->textures.tex[font->tex_index].texture.gltexnum;
    if (texid != -1)
    {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texid);
      CBaseShader_SetInt(gUIctx->shader, SH_UNIFORM_USE_TEXTURE, 1);
      CBaseShader_SetInt(gUIctx->shader, SH_UNIFORM_TEXTURE, 0);
    }else
    {
      CBaseShader_SetInt(gUIctx->shader, SH_UNIFORM_USE_TEXTURE, 0);
    }
  }


  glBindVertexArray(batch->vao);
  glBindBuffer(GL_ARRAY_BUFFER, batch->vbo);
  glBufferData(
      GL_ARRAY_BUFFER, 
      batch->vertexcount * sizeof(uivertex_t), 
      batch->vertices, 
      GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch->ebo);
  glBufferData(
      GL_ELEMENT_ARRAY_BUFFER,
      batch->indexcount * sizeof(GLuint),
      batch->indices,
      GL_DYNAMIC_DRAW
      );

  glEnableVertexAttribArray(0);
  // POS
  glVertexAttribPointer(
      0, 2,
      GL_FLOAT, GL_FALSE,
      sizeof(uivertex_t), (void*)OFFSETOF(uivertex_t, pos)
      );
  // UV
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
      1, 2,
      GL_FLOAT, GL_FALSE,
      sizeof(uivertex_t), (void*)OFFSETOF(uivertex_t, uv)
      );
  // Colour
  glEnableVertexAttribArray(2);
  glVertexAttribIPointer(
      2, 1,
      GL_UNSIGNED_INT, 
      sizeof(uivertex_t), (void*)OFFSETOF(uivertex_t, col)
      );

  glDrawElements(
      GL_TRIANGLES,
      (GLsizei)batch->indexcount,
      GL_UNSIGNED_INT,
      0
      );
  glBindVertexArray(0);
  batch->vertexcount = 0;
  batch->indexcount = 0;
}

void UI_DrawTextBatches()
{
  // Assume the shader is already bound, as called within UI_DrawBatch()
  for (int i = 0; i < gUIctx->textbatch_count; i++)
  {
    UI_DrawTextBatch(&gUIctx->textbatches[i]);
  }
}


void UI_DrawBatch()
{
  glDisable(GL_DEPTH_TEST);   // UI is 2D — ordering, not depth, decides visibility
  glDepthMask(GL_FALSE);      // don't let UI write into the depth buffer either

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // needed if you ever use alpha < 255



  // Bind shader
  if (!gUIctx->shader)
  {
    printf("[UI][DRAW]: Missing UI shader!!\n");
  }
  CBaseShader_Use(gUIctx->shader);
  vec2_t screensize;
  screensize[0] = gPltWindow->winw;
  screensize[1] = gPltWindow->winh;
  CBaseShader_SetVec2(gUIctx->shader, SH_UNIFORM_UI_SCREENSIZE, screensize);
  if (gUIctx->activetex == -1)
  {
    CBaseShader_SetInt(gUIctx->shader, SH_UNIFORM_USE_TEXTURE, 0);
  }
  else{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gUIctx->activetex);
    CBaseShader_SetInt(gUIctx->shader, SH_UNIFORM_USE_TEXTURE, 1);
    CBaseShader_SetInt(gUIctx->shader, SH_UNIFORM_TEXTURE, 1);
  }

  // Upload data
  glBindVertexArray(gUIctx->vao);
  // vbo
  glBindBuffer(GL_ARRAY_BUFFER, gUIctx->vbo);
  glBufferData(
      GL_ARRAY_BUFFER,
      gUIctx->vertexcount * sizeof(uivertex_t),
      gUIctx->vertices,
      GL_DYNAMIC_DRAW
      );
  // ebo
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gUIctx->ebo);
  glBufferData(
      GL_ELEMENT_ARRAY_BUFFER,
      gUIctx->indexcount * sizeof(GLuint),
      gUIctx->indices,
      GL_DYNAMIC_DRAW
      );
  
  // Vertex data
  
  // Position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
      0, 2, 
      GL_FLOAT, GL_FALSE, 
      sizeof(uivertex_t), (void*)OFFSETOF(uivertex_t, pos));
  // UV
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
      1, 2, 
      GL_FLOAT, GL_FALSE, 
      sizeof(uivertex_t), (void*)OFFSETOF(uivertex_t, uv));
  // colour
  glEnableVertexAttribArray(2);
  glVertexAttribIPointer(
      2, 1, 
      GL_UNSIGNED_INT, 
      sizeof(uivertex_t), (void*)OFFSETOF(uivertex_t, col));
  
  glDrawElements(
      GL_TRIANGLES, 
      (GLsizei)gUIctx->indexcount,
      GL_UNSIGNED_INT,
      0);

  glBindVertexArray(0);

  gUIctx->vertexcount = 0;
  gUIctx->indexcount  = 0;

  UI_DrawTextBatches();


  glDepthMask(GL_TRUE);       // restore state for whatever draws next (next frame's 3D pass)
  glEnable(GL_DEPTH_TEST);
}
