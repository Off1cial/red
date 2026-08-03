#pragma once

#include "corebase/mathlib.h"
#include <stdint.h>

#define ENTITY_MAX 2048

#include "shared/network/pframe.h"

typedef enum etype_t
{
  ENTITY_PLAYER,
  ENTITY_PROJECTILE,
  ENTITY_ENEMY,
} etype_t;

typedef struct entbase_t
{
  etype_t type;
  u32 id;
  u8 movflags;
  u8 movtype;
  vec3_t origin;
  vec3_t velocity;
  qangle angles;
} entbase_t;


// Temporary
#define MAX_WEAPONS 8

typedef struct entplayer_t
{
  entbase_t base;
  float health;
  u16 ammo[MAX_WEAPONS]; // Clip ammo
  u16 ammores[MAX_WEAPONS]; // Reserve ammo
  playercmd_t cmd;
} entplayer_t;


typedef struct entprojectile_t
{
  entbase_t base;
  u16 ownerid;
  float damage;
  float lifetime;

  // Func pointer to its destruction style?
  // Enum for destruction types? Fizzle, explode, fade ?

} entprojectile_t;


typedef struct entenemy_t
{
  entbase_t base;
  float health;
  // AI stuff.. maybe separate file
  
} entenemy_t;


typedef union 
{
  entbase_t base;
  entplayer_t player;
  entprojectile_t projectile;
  entenemy_t enemy;
} entity_t;

entity_t entities[ENTITY_MAX];

