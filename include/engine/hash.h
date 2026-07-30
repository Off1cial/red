// hash.h — shared by cvars, fonts, textures, anything else that wants a string->bucket table
#pragma once
#include <stdint.h>

static inline uint32_t Hash_String(const char* str)
{
    uint32_t h = 2166136261u; // FNV-1a offset basis
    while (*str) { h ^= (uint8_t)*str++; h *= 16777619u; } // FNV prime
    return h;
}

// Always use this when indexing into a fixed-size bucket array —
// never index with the raw hash directly.
static inline uint32_t Hash_Bucket(uint32_t hash, uint32_t bucket_count)
{
    return hash % bucket_count;
}