#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "hash_index.h"

/*
typedef enum EntryState
{
    EMPTY,
    OCCUPIED,
    TOMBSTONE
} EntryState;

typedef struct HashEntry
{
    uint64_t key;
    void *block;
    EntryState state;
} HashEntry;

typedef struct HashIndex
{
    size_t entry_count;
    size_t capacity;
    HashEntry *slots;
} HashIndex;
 */

static size_t hash_key(uint64_t key, size_t capacity)
{
    uint64_t hash = key;

    hash ^= hash >> 30;
    hash *= UINT64_C(0xbf58476d1ce4e5b9);

    hash ^= hash >> 27;
    hash *= UINT64_C(0x94d049bb133111eb);

    hash ^= hash >> 31;

    return (size_t)(hash % capacity);
}

int hi_init(HashIndex *index, size_t capacity)
{
    if (capacity == 0 || index == NULL)
    {
        return -1;
    }

    HashEntry *slots;

    if ((slots = malloc(capacity * sizeof(HashEntry))) == NULL)
    {
        return -1;
    }

    for (size_t i = 0; i < capacity; i++)
    {
        slots[i] = (HashEntry){.state = EMPTY};
    }

    index->entry_count = 0;
    index->capacity = capacity;
    index->slots = slots;

    return 0;
}

int hi_insert(HashIndex *index, uint64_t key, void *block)
{
    // Reject invalid block pointers and insertions into a full index.
    if (block == NULL || index == NULL)
    {
        return -1;
    }

    if (index->entry_count >= index->capacity)
    {
        return -1;
    }

    // Hashed key value and TOMBSTONE flags
    size_t ind = hash_key(key, index->capacity);
    size_t flag = 0;
    bool tomb_hit = false;

    // Linearly probes for EMPTY remember first TOMBSTONE
    for (size_t i = 0; i < index->capacity; ++i)
    {
        size_t temp = (i + ind) % index->capacity;

        if ((index->slots[temp].state == OCCUPIED) &&
            (index->slots[temp].key == key))
        {
            return -1;
        }

        // Notices a TOMBSTONE
        if ((index->slots[temp].state == TOMBSTONE) && tomb_hit == false)
        {
            flag = temp;
            tomb_hit = true;
        }

        if (index->slots[temp].state == EMPTY)
        {
            size_t dest = tomb_hit ? flag : temp;
            index->slots[dest].key = key;
            index->slots[dest].block = block;
            index->slots[dest].state = OCCUPIED;

            ++index->entry_count;
            return 0;
        }
    }

    if (tomb_hit)
    {
        index->slots[flag].key = key;
        index->slots[flag].block = block;
        index->slots[flag].state = OCCUPIED;

        ++index->entry_count;
        return 0;
    }

    // Returns failure because something went wrong
    return -1;
}

void *hi_lookup(const HashIndex *index, uint64_t key)
{
    // Guard prevents null pointers or empty indices
    if (index == NULL || index->entry_count == 0)
    {
        return NULL;
    }

    size_t ind = hash_key(key, index->capacity);

    for (size_t i = 0; i < index->capacity; ++i)
    {
        size_t temp = (i + ind) % index->capacity;

        if (index->slots[temp].state == EMPTY)
        {
            return NULL;
        }

        if ((index->slots[temp].state == OCCUPIED) &&
            (index->slots[temp].key == key))
        {
            return index->slots[temp].block;
        }
    }

    return NULL;
}

void *hi_remove(HashIndex *index, uint64_t key)
{
    if (index == NULL || index->entry_count == 0)
    {
        return NULL;
    }

    size_t ind = hash_key(key, index->capacity);

    for (size_t i = 0; i < index->capacity; ++i)
    {
        size_t temp = (i + ind) % index->capacity;

        if (index->slots[temp].state == EMPTY)
        {
            return NULL;
        }

        if ((index->slots[temp].state == OCCUPIED) &&
            (index->slots[temp].key == key))
        {
            void *lookup = index->slots[temp].block;

            index->slots[temp].key = 0;
            index->slots[temp].block = NULL;
            index->slots[temp].state = TOMBSTONE;

            --index->entry_count;

            return lookup;
        }
    }

    return NULL;
}

/*
typedef enum EntryState
{
    EMPTY,
    OCCUPIED,
    TOMBSTONE
} EntryState;

typedef struct HashEntry
{
    uint64_t key;
    void *block;
    EntryState state;
} HashEntry;

typedef struct HashIndex
{
    size_t entry_count;
    size_t capacity;
    HashEntry *slots;
} HashIndex;
 */

int hi_destroy(HashIndex *index)
{
    if (index == NULL)
    {
        return -1;
    }

    free(index->slots);

    index->entry_count = 0;
    index->capacity = 0;
    index->slots = NULL;

    return 0;
}
