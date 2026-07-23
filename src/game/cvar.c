#include "game/cvar.h"
#include "platform/common.h"

#ifndef SEED_PLACEHOLDER
#define SEED_PLACEHOLDER 0
#endif


#define HASH_BUCKET_COUNT 256

cvar_t cvar_indexes[CVAR_MAXCVARS];
int    cvar_numIndexes;

static cvar_t* cvarHashTable[HASH_BUCKET_COUNT];


static inline uint64_t cvar_hash(const char* name, uint64_t seed)
{
  uint32_t h = seed ? seed : 2166136261u;
  while (*name) { h ^= (uint8_t)*name++; h *= 16777619u; }
  return h;
}


static cvar_t* Cvar_Find(const char* name)
{
  uint64_t hash;
  cvar_t* var;

  hash = cvar_hash(name, SEED_PLACEHOLDER);

  for (var=cvarHashTable[hash]; var; var=var->hashNext)
  {
    if (!Q_stricmp(var->name, name))
    {
      return var;
    }
  }
  return NULL;
}

void Cvar_Register(const char* name, const char* defaultValue, uint64_t flags)
{
  // Does the Cvar already exist
  cvar_t* cv;

  cv = Cvar_Find(name);
  if (!cv)
    return;
  
  // Cvar is new
  uint64_t hash;
  cv = &cvar_indexes[cvar_numIndexes++];
  Q_strncpy(cv->name, name, strlen(name));
  Q_strncpy(cv->string, defaultValue, strlen(defaultValue));
  cv->value = atof(cv->string);
  cv->value_int = atoi(cv->string);
  cv->flags = flags;

  hash = cvar_hash(name, SEED_PLACEHOLDER); 
  cv->hashNext = cvarHashTable[hash];

  

  cvarHashTable[hash] = cv;
}


char* Cvar_ValueString(const char* name)
{
  cvar_t* cv;
  cv = Cvar_Find(name);
  if (!cv)
    return "";

  return cv->name;
}

float Cvar_ValueFloat(const char* name)
{
  cvar_t* cv;
  cv = Cvar_Find(name);
  if (!cv)
    return 0.0f;

  return cv->value;
}

int Cvar_ValueInteger(const char* name)
{
  cvar_t* cv;
  cv = Cvar_Find(name);
  if (!cv)
    return 0;
  
  return cv->value_int;
}


void Cvar_InitAll()
{
  Cvar_Register("sensitivity", "1.0", CVAR_ARCHIVE | CVAR_CLIENT);
}
