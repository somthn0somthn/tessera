// Needed for alignof(), malloc(), and uintptr_t
#include <stdalign.h>
#include <stdint.h>
#include <stdlib.h>

#include "block_pool.h"

struct MemoryBlock
{
    struct MemoryBlock *next;
};

// Initializes memory pool with block_count blocks of size block_size
int block_pool_init(MemoryPool *pool, size_t block_size, size_t block_count)
{
    // Guards that each block can hold a MemoryBlock, that block_size preserves
    // MemoryBlock alignment, and that block_count is nonzero.
    if ((block_size < sizeof(MemoryBlock)) ||
        block_size % alignof(MemoryBlock) != 0 || block_count == 0)
        return -1;

    // Establishes what will indicate the first free_head of the pool and the
    // address of the allocated memory block
    MemoryBlock *current_block;

    // Allocates block_size * block_count bytes of memory
    if ((current_block = malloc(block_size * block_count)) == NULL)
        return -1;

    // Initializes pool struct values
    pool->block_size = block_size;
    pool->block_count = block_count;
    pool->allocated_count = 0;
    pool->free_head = current_block;
    pool->base_ptr = current_block;

    // Links MemoryBlocks within pool
    for (size_t i = 0; i < block_count - 1; i++)
    {
        // Cast to char * so pointer arithmetic advances in bytes,
        // since block_size is expressed in bytes.
        current_block->next =
            (MemoryBlock *)((char *)current_block + block_size);
        // Chains blocks
        current_block = current_block->next;
    }

    // Sets final block->next pointer to NULL and returns 0 for success
    current_block->next = NULL;
    return 0;
}

void *block_pool_alloc(MemoryPool *pool)
{
    if (pool->free_head == NULL)
        return NULL;

    void *ptr = pool->free_head;
    MemoryBlock *next = pool->free_head->next;

    pool->free_head = next;
    pool->allocated_count++;

    return ptr;
}

// Returns a previously allocated block to the pool.
int block_pool_free(MemoryPool *pool, void *ptr)
{
    uintptr_t base_address = (uintptr_t)pool->base_ptr;
    uintptr_t candidate_address = (uintptr_t)ptr;
    uintptr_t end_address = base_address + pool->block_size * pool->block_count;
    if (candidate_address < base_address || candidate_address >= end_address)
        return -1;

    size_t offset = candidate_address - base_address;

    if ((pool->allocated_count == 0) || (offset % pool->block_size) != 0)
        return -1;

    MemoryBlock *block = ptr;
    block->next = pool->free_head;
    pool->free_head = block;
    pool->allocated_count--;

    return 0;
}

int block_pool_destroy(MemoryPool *pool)
{
    if (pool->allocated_count != 0)
        return -1;

    free(pool->base_ptr);

    pool->block_size = 0;
    pool->block_count = 0;
    pool->allocated_count = 0;
    pool->free_head = NULL;
    pool->base_ptr = NULL;

    return 0;
}