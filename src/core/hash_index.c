// Needed for malloc, uint64_t, and bool types
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "hash_index.h"

static const uint64_t MIX1 = UINT64_C(0xbf58476d1ce4e5b9);
static const uint64_t MIX2 = UINT64_C(0x94d049bb133111eb);

// Key hashing function that repeatedly shifts, XORs, and multiplies the key
// value. Modeled after the SplitMix64 mixing function
static size_t hash_key(uint64_t key, size_t capacity)
{
    uint64_t hash = key;

    hash ^= hash >> 30;
    hash *= MIX1;

    hash ^= hash >> 27;
    hash *= MIX2;

    hash ^= hash >> 31;

    return (size_t)(hash % capacity);
}

// Initializes a HashIndex of size capacity
int hi_init(HashIndex *index, size_t capacity)
{
    // Guards for invalid arguments
    if (capacity == 0 || index == NULL)
    {
        return -1;
    }

    HashEntry *slots;

    // Allocates memory for slots array, exiting upon failure
    if ((slots = malloc(capacity * sizeof(HashEntry))) == NULL)
    {
        return -1;
    }

    // Initializes each slot in the array with a HashEntry of state EMPTY
    for (size_t i = 0; i < capacity; i++)
    {
        slots[i] = (HashEntry){.state = EMPTY};
    }

    // Sets index metadata and returns success
    index->entry_count = 0;
    index->capacity = capacity;
    index->slots = slots;

    return 0;
}

// Inserts a key and block in HashIndex index
int hi_insert(HashIndex *index, uint64_t key, void *block)
{
    // Reject invalid block pointers and insertions into a full index
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
    size_t flag_index = 0;
    bool tomb_hit = false;

    // Linearly probes for EMPTY remembering first TOMBSTONE hit
    for (size_t i = 0; i < index->capacity; ++i)
    {
        // Wrap probing around to the beginning of the table
        size_t temp = (i + ind) % index->capacity;

        // Reject duplicate keys
        if ((index->slots[temp].state == OCCUPIED) &&
            (index->slots[temp].key == key))
        {
            return -1;
        }

        // Notices a TOMBSTONE and sets the tomb_hit flag to true and flag_index
        // to the TOMBSTONE index value
        if ((index->slots[temp].state == TOMBSTONE) && !tomb_hit)
        {
            flag_index = temp;
            tomb_hit = true;
        }

        // Notices and slot of state EMPTY. If tomb_hit has been set to true, it
        // assigns the key and block to the flag_index slot of the slots array.
        // Otherwise, its sets the current EMPTY to slot to the key-block value.
        // Either case is marked OCCUPIED and then returns.
        if (index->slots[temp].state == EMPTY)
        {
            size_t dest = tomb_hit ? flag_index : temp;
            index->slots[dest].key = key;
            index->slots[dest].block = block;
            index->slots[dest].state = OCCUPIED;

            ++index->entry_count;
            return 0;
        }
    }

    // If no EMPTY value was hit but a TOMBSTONE was, the key-block value is
    // entered at slot position flag_index and the state is set to OCCUPIED
    if (tomb_hit)
    {
        index->slots[flag_index].key = key;
        index->slots[flag_index].block = block;
        index->slots[flag_index].state = OCCUPIED;

        ++index->entry_count;
        return 0;
    }

    // Returns failure because something went wrong
    return -1;
}

// Looks up key in index, returning a pointer to the associated block on a hit
void *hi_lookup(const HashIndex *index, uint64_t key)
{
    // Guard prevents null pointers or empty indices
    if (index == NULL || index->entry_count == 0)
    {
        return NULL;
    }

    // Gets the initial slot value from the hashed key
    size_t ind = hash_key(key, index->capacity);

    // Probe from the hashed index, wrapping around the table until the key or
    // an EMPTY slot is encountered
    for (size_t i = 0; i < index->capacity; ++i)
    {
        // Wrap probing around to the beginning of the table
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

    // Return NULL if every slot is probed without finding the key
    return NULL;
}

// Removes a key and associated block from the HashIndex, returning the
// associated block
void *hi_remove(HashIndex *index, uint64_t key)
{
    // Guard prevents null pointers or empty indices
    if (index == NULL || index->entry_count == 0)
    {
        return NULL;
    }

    // Gets the initial slot value from the hashed key
    size_t ind = hash_key(key, index->capacity);

    // Probe from the hashed index, wrapping around the table until the key or
    // an EMPTY slot is encountered
    for (size_t i = 0; i < index->capacity; ++i)
    {
        // Wrap probing around to the beginning of the table
        size_t temp = (i + ind) % index->capacity;

        // Returns NULL upon encountering EMPTY
        if (index->slots[temp].state == EMPTY)
        {
            return NULL;
        }

        // Returns block on a hit, clears the entry, and marks the slot
        // TOMBSTONE
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

    // Return NULL if every slot is probed without finding the key
    return NULL;
}

// Destroys the HashIndex
int hi_destroy(HashIndex *index)
{
    // Returns failure if index is NULL
    if (index == NULL)
    {
        return -1;
    }

    // Frees the index slot array and resets the HashIndex metadata
    free(index->slots);

    index->entry_count = 0;
    index->capacity = 0;
    index->slots = NULL;

    return 0;
}
