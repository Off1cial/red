#include "game/cvar.h"
#include "platform/common.h"
#include "engine/hash.h"

#define HASH_BUCKET_COUNT 256

cvar_t cvar_indexes[CVAR_MAXCVARS];
int    cvar_numIndexes;

static cvar_t* cvarHashTable[HASH_BUCKET_COUNT];

static cvar_t* Cvar_Find(const char* name)
{
    uint32_t bucket = Hash_Bucket(Hash_String(name), HASH_BUCKET_COUNT);

    for (cvar_t* var = cvarHashTable[bucket]; var; var = var->hashNext)
    {
        if (!Q_stricmp(var->name, name))
            return var;
    }
    return NULL;
}

void Cvar_Register(const char* name, const char* defaultValue, uint64_t flags)
{
    if (Cvar_Find(name))
        return;

    if (cvar_numIndexes >= CVAR_MAXCVARS)
    {
        printf("[CVAR]: cvar_indexes full, cannot register '%s'\n", name);
        return;
    }

    cvar_t* cv = &cvar_indexes[cvar_numIndexes++];
    memset(cv, 0, sizeof(cvar_t));

    // Q_strncpy(dst, src, strlen(src)) never null-terminates unless dst
    // happens to already be zeroed AND src is shorter than dst's capacity.
    // Size off the DESTINATION buffer, and terminate explicitly.
    size_t namelen = strlen(name);
    if (namelen >= sizeof(cv->name)) namelen = sizeof(cv->name) - 1;
    memcpy(cv->name, name, namelen);
    cv->name[namelen] = '\0';

    size_t vallen = strlen(defaultValue);
    if (vallen >= sizeof(cv->string)) vallen = sizeof(cv->string) - 1;
    memcpy(cv->string, defaultValue, vallen);
    cv->string[vallen] = '\0';

    cv->value = atof(cv->string);
    cv->value_int = atoi(cv->string);
    cv->flags = flags;

    uint32_t bucket = Hash_Bucket(Hash_String(name), HASH_BUCKET_COUNT);
    cv->hashNext = cvarHashTable[bucket];
    cvarHashTable[bucket] = cv;
}

const char* Cvar_ValueString(const char* name)
{
    cvar_t* cv = Cvar_Find(name);
    if (!cv) return "";
    return cv->string;
}

float Cvar_ValueFloat(const char* name)
{
    cvar_t* cv = Cvar_Find(name);
    return cv ? cv->value : 0.0f;
}

int Cvar_ValueInteger(const char* name)
{
    cvar_t* cv = Cvar_Find(name);
    return cv ? cv->value_int : 0;
}

void Cvar_InitAll()
{
    Cvar_Register("sensitivity", "1.0", CVAR_ARCHIVE | CVAR_CLIENT);
}