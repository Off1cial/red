#pragma once

#include "corebase/mathlib.h"
#include <stdint.h>

#define ENTITY_MAX 2048

typedef enum etype_t
{
  ENTITY_PLAYER,
  ENTITY_PROJECTILE,
  ENTITY_ENEMY,
} etype_t;





typedef struct epair_s
{
  struct epair_s *next;
  char *key;
  char *value;
  

} epair_t;

typedef struct entvars_s
{ 
  vec3_t origin;
  vec3_t velocity;
  qangle angles;
  uint8_t movtype;
  uint8_t movflags;

    
} entvars_t;

typedef struct entity_s
{
  etype_t type;
  epair_t pair;
  entvars_t v;
} entity_t;


typedef struct edict_s
{
  epair_t data[ENTITY_MAX];
} edict_t;
