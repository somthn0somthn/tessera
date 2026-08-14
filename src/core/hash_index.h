#ifndef TESSERA_HASH_INDEX_H
#define TESSERA_HASH_INDEX_H

// Needed for size_t and uint64_t
#include <stddef.h>
#include <stdint.h>

typedef enum EntryState {
    EMPTY,
    OCCUPIED,
    TOMBSTONE
} EntryState;

typedef struct HashEntry {
    uint64_t key;
    void *block;
    EntryState state;
} HashEntry;

typedef struct HashIndex {
    size_t entry_count;
    size_t capacity;
    HashEntry *table;
} HashIndex;

int hi_init(HashIndex *index, size_t capacity);

int hi_insert(HashIndex *index, uint64_t key, void *block);

void *hi_lookup(HashIndex *index, uint64_t key);

void *hi_remove(HashIndex *index, uint64_t key);

int hi_destroy(HashIndex *index);

int hi_size(HashIndex *index); 

#endif