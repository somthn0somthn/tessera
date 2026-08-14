#include <stdio.h>
#include <assert.h>

#include "core/block_pool.h"

static void test_pool_init(void)
{
    MemoryPool pool;
    assert(block_pool_init(&pool, 64, 4) == 0);

    assert(block_pool_destroy(&pool) == 0);
}

static void test_pool_invalid_init(void)
{
    MemoryPool pool;
    assert(block_pool_init(&pool, 0, 4) == -1);
    assert(block_pool_init(&pool, 13, 4) == -1);
    assert(block_pool_init(&pool, 64, 0) == -1);
}

static void test_pool_alloc(void)
{
    MemoryPool pool;
    assert(block_pool_init(&pool, 64, 4) == 0);

    void *ptr = block_pool_alloc(&pool);
    assert(ptr != NULL);

    assert(block_pool_free(&pool, ptr) == 0);
    assert(block_pool_destroy(&pool) == 0);
}

static void test_pool_exhaust(void)
{
    MemoryPool pool;
    assert(block_pool_init(&pool, 64, 4) == 0);

    void *ptr0 = block_pool_alloc(&pool);
    assert(ptr0 != NULL);
    void *ptr1 = block_pool_alloc(&pool);
    assert(ptr1 != NULL);
    void *ptr2 = block_pool_alloc(&pool);
    assert(ptr2 != NULL);
    void *ptr3 = block_pool_alloc(&pool);
    assert(ptr3 != NULL);
    assert(pool.allocated_count == 4);

    void *ptr4 = block_pool_alloc(&pool);
    assert(ptr4 == NULL);

    assert(block_pool_free(&pool, ptr0) == 0);
    assert(block_pool_free(&pool, ptr1) == 0);
    assert(block_pool_free(&pool, ptr2) == 0);
    assert(block_pool_free(&pool, ptr3) == 0);

    assert(block_pool_destroy(&pool) == 0);
}

static void test_free_decrease_alloc_count(void)
{
    MemoryPool pool;
    assert(block_pool_init(&pool, 64, 4) == 0);

    void *ptr0 = block_pool_alloc(&pool);
    assert(ptr0 != NULL);
    void *ptr1 = block_pool_alloc(&pool);
    assert(ptr1 != NULL);

    assert(pool.allocated_count == 2);

    assert(block_pool_free(&pool, ptr0) == 0);
    assert(pool.allocated_count == 1);

    assert(block_pool_free(&pool, ptr1) == 0);
    assert(pool.allocated_count == 0);

    assert(block_pool_destroy(&pool) == 0);
}

static void test_free_block_reused(void)
{
    MemoryPool pool;
    assert(block_pool_init(&pool, 64, 4) == 0);

    void *ptr0 = block_pool_alloc(&pool);
    assert(ptr0 != NULL);

    void *ptr1 = block_pool_alloc(&pool);
    assert(ptr1 != NULL);
    assert(block_pool_free(&pool, ptr0) == 0);

    void *ptr2 = block_pool_alloc(&pool);
    assert(ptr2 != NULL);

    assert(ptr0 == ptr2);

    assert(block_pool_free(&pool, ptr1) == 0);
    assert(block_pool_free(&pool, ptr2) == 0);

    assert(block_pool_destroy(&pool) == 0);
}

static void test_destroy_fails_on_nonempty(void)
{
    MemoryPool pool;
    assert(block_pool_init(&pool, 64, 4) == 0);

    void *ptr0 = block_pool_alloc(&pool);
    assert(ptr0 != NULL);
    void *ptr1 = block_pool_alloc(&pool);
    assert(ptr1 != NULL);

    assert(block_pool_destroy(&pool) == -1);

    assert(block_pool_free(&pool, ptr0) == 0);
    assert(block_pool_destroy(&pool) == -1);

    assert(block_pool_free(&pool, ptr1) == 0);
    assert(block_pool_destroy(&pool) == 0);
}

int main(void)
{
    test_pool_init();
    test_pool_invalid_init();
    test_pool_alloc();
    test_pool_exhaust();
    test_free_decrease_alloc_count();
    test_free_block_reused();
    test_destroy_fails_on_nonempty();

    puts("All block_pool tests passed.");
    return 0;
}