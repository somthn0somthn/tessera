#ifndef TESSERA_HASH_INDEX_H
#define TESSERA_HASH_INDEX_H

// Needed for size_t and uint64_t
#include <stddef.h>
#include <stdint.h>

//  Slot states used by the open-addressed hash index.
typedef enum EntryState
{
    EMPTY,
    OCCUPIED,
    TOMBSTONE
} EntryState;

// Represents one slot in the hash index. Each occupied slot maps a non-negative
// 64-bit key to a block pointer.
typedef struct HashEntry
{
    uint64_t key;
    void *block;
    EntryState state;
} HashEntry;

// HashIndex owns an array of HashEntry slots and tracks table metadata.
typedef struct HashIndex
{
    size_t entry_count;
    size_t capacity;
    HashEntry *slots;
} HashIndex;

int hi_init(HashIndex *index, size_t capacity);

int hi_insert(HashIndex *index, uint64_t key, void *block);

void *hi_lookup(const HashIndex *index, uint64_t key);

void *hi_remove(HashIndex *index, uint64_t key);

int hi_destroy(HashIndex *index);

static inline size_t hi_size(const HashIndex *index)
{
    return index->entry_count;
}

#endif
