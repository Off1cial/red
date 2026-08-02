#pragma once

#include "corebase/mathlib.h"

#define ENTITY_MAX 2048

typedef enum etype_t
{
  ENTITY_PLAYER,
  ENTITY_PROJECTILE,
  ENTITY_ENEMY,
} etype_t;



typedef struct entity_state_t
{
  vec3_t origin;
  qangle angles;
  // int model
  // int anim
  // int animframe
  // uint64_t effects
  // etc..

} entity_state_t;


typedef struct epair_s
{
  struct epair_s *next;
  char *key;
  char *value;
  

} epair_t;

typedef struct entity_s
{
  etype_t type;
  entity_state_t state;
  epair_t pair;
  
} entity_t;


typedef struct edict_s
{
  epair_t data[ENTITY_MAX];
} edict_t;
