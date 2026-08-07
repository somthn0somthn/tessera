#ifndef BLOCK_POOL_H
#define BLOCK_POOL_H

// Needed for size_t.
#include <stddef.h>

// Intrusive linked-list node used to organize free blocks.
// While a block is free, its first bytes store a pointer to the next free block.
typedef struct MemoryBlock {
    struct MemoryBlock *next;
} MemoryBlock;

// MemoryPool stores pool metadata, the current free-list head,
// and the base address of the backing allocation.
typedef struct MemoryPool {
    size_t block_size;
    size_t block_count;
    size_t allocated_count;
    MemoryBlock *free_head;
    void *base_ptr;
} MemoryPool;

// Allocates the backing memory and initializes the pool.
// The pool retains ownership of the backing allocation; allocate_block()
// grants the caller exclusive use of one block.
MemoryPool *create_pool(size_t block_size, size_t block_count);

// Returns a pointer to one available block, or NULL if none are available.
void *allocate_block(MemoryPool *pool);

// Returns a previously allocated block to the pool.
void free_block(MemoryPool *pool, void *ptr);

// Releases the pool and its backing allocation.
// Fails if blocks are still checked out by callers.
int destroy_pool(MemoryPool *pool);

#endif