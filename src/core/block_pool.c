#include <stdalign.h>
#include <stdint.h>

#include "block_pool.h"

// typedef struct MemoryPool
// {
//     size_t block_size;
//     size_t block_count;
//     size_t allocated_count;
//     MemoryBlock *free_head;
//     void *base_ptr;
// } MemoryPool;

// typedef struct MemoryBlock
// {
//     struct MemoryBlock *next;
// } MemoryBlock;

int init_pool(MemoryPool *pool, size_t block_size, size_t block_count)
{

    if ((block_size < sizeof(MemoryBlock)) ||
        block_size % alignof(MemoryBlock) != 0 || block_count == 0)
        return -1;

    MemoryBlock *base_ptr;

    if ((base_ptr = (MemoryBlock *)malloc(block_size * block_count)) == NULL)
        return -1;

    pool->block_size = block_size;
    pool->block_count = block_count;
    pool->allocated_count = 0;
    pool->free_head = base_ptr;
    pool->base_ptr = (void *)base_ptr;

    for (size_t i = 0; i < block_count - 1; i++)
    {
        base_ptr->next = (MemoryBlock *)((char *)base_ptr + block_size);
        base_ptr = (MemoryBlock *)base_ptr->next;
    }

    base_ptr->next = NULL;
    return 0;
}

void *allocate_block(MemoryPool *pool)
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
int free_block(MemoryPool *pool, void *ptr)
{
    uintptr_t base_address = (uintptr_t)pool->base_ptr;
    uintptr_t candidate_address = (uintptr_t)ptr;
    uintptr_t end_address = base_address + pool->block_size * pool->block_count;
    if (candidate_address < base_address || candidate_address >= end_address)
        return -1;

    size_t offset = candidate_address - base_address;

    if ((pool->allocated_count < 1) || (offset % pool->block_size) != 0)
        return -1;

    MemoryBlock *block = (MemoryBlock *)ptr;
    block->next = pool->free_head;
    pool->free_head = block;
    pool->allocated_count--;

    return 0;
}