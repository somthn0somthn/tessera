#ifndef TESSERA_BLOCK_POOL_H
#define TESSERA_BLOCK_POOL_H

// Needed for size_t.
#include <stddef.h>

// Intrusive linked-list node used to organize free blocks.
// While a block is free, its first bytes store a pointer to the next free
// block.
typedef struct MemoryBlock MemoryBlock;

// MemoryPool stores pool metadata, the current free-list head,
// and the base address of the backing allocation.
typedef struct MemoryPool
{
    size_t block_size;
    size_t block_count;
    size_t allocated_count;
    MemoryBlock *free_head;
    void *base_ptr;
} MemoryPool;

// Allocates the backing memory and initializes the pool.
// The pool retains ownership of the backing allocation; block_pool_alloc()
// grants the caller exclusive use of one block.
int block_pool_init(MemoryPool *pool, size_t block_size, size_t block_count);

// Returns a pointer to one available block, or NULL if none are available.
void *block_pool_alloc(MemoryPool *pool);

// Returns a previously allocated block to the pool.
int block_pool_free(MemoryPool *pool, void *ptr);

// Releases the pool and its backing allocation.
// Fails if blocks are still checked out by callers.
int block_pool_destroy(MemoryPool *pool);

#endif
