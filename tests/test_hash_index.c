#include <assert.h>

#include "core/hash_index.h"
#include "core/block_pool.h"

static void test_hi_init(void)
{
    HashIndex index;
    size_t capacity = 1000;

    assert(hi_init(&index, capacity) == 0);
    assert(hi_size(&index) == 0);
    assert(hi_destroy(&index) == 0);
}

static void test_hi_invalid_init(void)
{
    HashIndex index;

    assert(hi_init(NULL, 1000) == -1);
    assert(hi_init(&index, 0) == -1);
}

static void test_hi_insert_lookup(void)
{
    MemoryPool pool;
    HashIndex index;
    uint64_t key = 100;
    size_t capacity = 1000;
    size_t block_size = 64;
    size_t block_count = 16;

    assert(block_pool_init(&pool, block_size, block_count) == 0);

    void *block_ptr = block_pool_alloc(&pool);
    assert(block_ptr != NULL);

    assert(hi_init(&index, capacity) == 0);
    assert(hi_insert(&index, key, block_ptr) == 0);
    assert(hi_size(&index) == 1);

    assert(hi_lookup(&index, key) == block_ptr);

    assert(hi_destroy(&index) == 0);
    assert(block_pool_free(&pool, block_ptr) == 0);
    assert(block_pool_destroy(&pool) == 0);
}

static void test_hi_multiple_entries(void)
{
    MemoryPool pool;
    HashIndex index;
    uint64_t key1 = 100;
    uint64_t key2 = 202;
    uint64_t key3 = 330;
    size_t capacity = 1000;
    size_t block_size = 64;
    size_t block_count = 16;

    assert(block_pool_init(&pool, block_size, block_count) == 0);

    void *block1_ptr = block_pool_alloc(&pool);
    assert(block1_ptr != NULL);
    void *block2_ptr = block_pool_alloc(&pool);
    assert(block2_ptr != NULL);
    void *block3_ptr = block_pool_alloc(&pool);
    assert(block3_ptr != NULL);

    assert(hi_init(&index, capacity) == 0);
    assert(hi_insert(&index, key1, block1_ptr) == 0);
    assert(hi_insert(&index, key2, block2_ptr) == 0);
    assert(hi_insert(&index, key3, block3_ptr) == 0);
    assert(hi_size(&index) == 3);

    assert(hi_lookup(&index, key1) == block1_ptr);
    assert(hi_lookup(&index, key2) == block2_ptr);
    assert(hi_lookup(&index, key3) == block3_ptr);

    assert(hi_destroy(&index) == 0);

    assert(block_pool_free(&pool, block1_ptr) == 0);
    assert(block_pool_free(&pool, block2_ptr) == 0);
    assert(block_pool_free(&pool, block3_ptr) == 0);

    assert(block_pool_destroy(&pool) == 0);
}

static void test_hi_lookup_missing(void)
{
    MemoryPool pool;
    HashIndex index;
    uint64_t key1 = 100;
    uint64_t key2 = 202;
    uint64_t key3 = 330;
    size_t capacity = 1000;
    size_t block_size = 64;
    size_t block_count = 16;

    assert(block_pool_init(&pool, block_size, block_count) == 0);

    void *block1_ptr = block_pool_alloc(&pool);
    assert(block1_ptr != NULL);
    void *block2_ptr = block_pool_alloc(&pool);
    assert(block2_ptr != NULL);

    assert(hi_init(&index, capacity) == 0);
    assert(hi_insert(&index, key1, block1_ptr) == 0);
    assert(hi_insert(&index, key2, block2_ptr) == 0);
    assert(hi_size(&index) == 2);

    assert(hi_lookup(&index, key1) == block1_ptr);
    assert(hi_lookup(&index, key2) == block2_ptr);
    assert(hi_lookup(&index, key3) == NULL);

    assert(hi_destroy(&index) == 0);

    assert(block_pool_free(&pool, block1_ptr) == 0);
    assert(block_pool_free(&pool, block2_ptr) == 0);

    assert(block_pool_destroy(&pool) == 0);
}

static void test_hi_duplicate_rejected(void)
{
    MemoryPool pool;
    HashIndex index;
    uint64_t key = 100;
    size_t capacity = 1000;
    size_t block_size = 64;
    size_t block_count = 16;

    assert(block_pool_init(&pool, block_size, block_count) == 0);

    void *block1_ptr = block_pool_alloc(&pool);
    assert(block1_ptr != NULL);
    void *block2_ptr = block_pool_alloc(&pool);
    assert(block2_ptr != NULL);

    assert(hi_init(&index, capacity) == 0);
    assert(hi_insert(&index, key, block1_ptr) == 0);
    assert(hi_insert(&index, key, block2_ptr) == -1);
    assert(hi_size(&index) == 1);

    assert(hi_lookup(&index, key) == block1_ptr);

    assert(hi_destroy(&index) == 0);

    assert(block_pool_free(&pool, block1_ptr) == 0);
    assert(block_pool_free(&pool, block2_ptr) == 0);

    assert(block_pool_destroy(&pool) == 0);
}

static void test_hi_full(void)
{

    MemoryPool pool;
    HashIndex index;
    uint64_t key1 = 100;
    uint64_t key2 = 202;
    uint64_t key3 = 330;
    uint64_t key4 = 4004;
    size_t capacity = 3;
    size_t block_size = 64;
    size_t block_count = 16;

    assert(block_pool_init(&pool, block_size, block_count) == 0);

    void *block1_ptr = block_pool_alloc(&pool);
    assert(block1_ptr != NULL);
    void *block2_ptr = block_pool_alloc(&pool);
    assert(block2_ptr != NULL);
    void *block3_ptr = block_pool_alloc(&pool);
    assert(block3_ptr != NULL);
    void *block4_ptr = block_pool_alloc(&pool);
    assert(block4_ptr != NULL);

    assert(hi_init(&index, capacity) == 0);
    assert(hi_insert(&index, key1, block1_ptr) == 0);
    assert(hi_insert(&index, key2, block2_ptr) == 0);
    assert(hi_insert(&index, key3, block3_ptr) == 0);
    assert(hi_insert(&index, key4, block4_ptr) == -1);
    assert(hi_size(&index) == 3);

    assert(hi_destroy(&index) == 0);

    assert(block_pool_free(&pool, block1_ptr) == 0);
    assert(block_pool_free(&pool, block2_ptr) == 0);
    assert(block_pool_free(&pool, block3_ptr) == 0);
    assert(block_pool_free(&pool, block4_ptr) == 0);

    assert(block_pool_destroy(&pool) == 0);
}

static void test_hi_remove(void)
{
    MemoryPool pool;
    HashIndex index;
    uint64_t key = 100;
    size_t capacity = 1000;
    size_t block_size = 64;
    size_t block_count = 16;

    assert(block_pool_init(&pool, block_size, block_count) == 0);

    void *block_ptr = block_pool_alloc(&pool);
    assert(block_ptr != NULL);

    assert(hi_init(&index, capacity) == 0);
    assert(hi_insert(&index, key, block_ptr) == 0);
    assert(hi_size(&index) == 1);

    assert(hi_remove(&index, key) == block_ptr);
    assert(hi_size(&index) == 0);
    assert(hi_lookup(&index, key) == NULL);

    assert(hi_destroy(&index) == 0);
    assert(block_pool_free(&pool, block_ptr) == 0);
    assert(block_pool_destroy(&pool) == 0);
}

static void test_hi_lookup_through_tombstone(void)
{
    MemoryPool pool;
    HashIndex index;
    uint64_t key1 = 4;
    uint64_t key2 = 5;
    size_t capacity = 8;
    size_t block_size = 64;
    size_t block_count = 16;

    assert(block_pool_init(&pool, block_size, block_count) == 0);

    void *block_ptr1 = block_pool_alloc(&pool);
    assert(block_ptr1 != NULL);
    void *block_ptr2 = block_pool_alloc(&pool);
    assert(block_ptr2 != NULL);

    assert(hi_init(&index, capacity) == 0);
    assert(hi_insert(&index, key1, block_ptr1) == 0);
    assert(hi_insert(&index, key2, block_ptr2) == 0);
    assert(hi_size(&index) == 2);

    assert(hi_remove(&index, key1) == block_ptr1);
    assert(hi_lookup(&index, key2) == block_ptr2);

    assert(hi_destroy(&index) == 0);
    assert(block_pool_free(&pool, block_ptr1) == 0);
    assert(block_pool_free(&pool, block_ptr2) == 0);
    assert(block_pool_destroy(&pool) == 0);
}

static void test_hi_tombstone_reused(void)
{

    MemoryPool pool;
    HashIndex index;
    uint64_t key1 = 4;
    uint64_t key2 = 5;
    uint64_t key3 = 6;
    uint64_t key4 = 7;
    size_t capacity = 8;
    size_t block_size = 64;
    size_t block_count = 16;

    assert(block_pool_init(&pool, block_size, block_count) == 0);

    void *block_ptr1 = block_pool_alloc(&pool);
    assert(block_ptr1 != NULL);
    void *block_ptr2 = block_pool_alloc(&pool);
    assert(block_ptr2 != NULL);
    void *block_ptr3 = block_pool_alloc(&pool);
    assert(block_ptr3 != NULL);
    void *block_ptr4 = block_pool_alloc(&pool);
    assert(block_ptr4 != NULL);

    assert(hi_init(&index, capacity) == 0);
    assert(hi_insert(&index, key1, block_ptr1) == 0);
    assert(hi_insert(&index, key2, block_ptr2) == 0);
    assert(hi_insert(&index, key3, block_ptr3) == 0);
    assert(hi_size(&index) == 3);

    assert(hi_remove(&index, key2) == block_ptr2);
    assert(hi_insert(&index, key4, block_ptr4) == 0);

    assert(index.slots[5].key == key4);
    assert(index.slots[5].block == block_ptr4);
    assert(index.slots[5].state == OCCUPIED);

    assert(hi_destroy(&index) == 0);
    assert(block_pool_free(&pool, block_ptr1) == 0);
    assert(block_pool_free(&pool, block_ptr2) == 0);
    assert(block_pool_free(&pool, block_ptr3) == 0);
    assert(block_pool_free(&pool, block_ptr4) == 0);
    assert(block_pool_destroy(&pool) == 0);
}

static void test_hi_probe_wraparound(void)
{
    MemoryPool pool;
    HashIndex index;
    uint64_t key1 = 9;
    uint64_t key2 = 19;
    uint64_t key3 = 33;
    size_t capacity = 8;
    size_t block_size = 64;
    size_t block_count = 16;

    assert(block_pool_init(&pool, block_size, block_count) == 0);

    void *block_ptr1 = block_pool_alloc(&pool);
    assert(block_ptr1 != NULL);
    void *block_ptr2 = block_pool_alloc(&pool);
    assert(block_ptr2 != NULL);
    void *block_ptr3 = block_pool_alloc(&pool);
    assert(block_ptr3 != NULL);

    assert(hi_init(&index, capacity) == 0);
    assert(hi_insert(&index, key1, block_ptr1) == 0);
    assert(hi_insert(&index, key2, block_ptr2) == 0);
    assert(hi_insert(&index, key3, block_ptr3) == 0);
    assert(hi_size(&index) == 3);

    assert(index.slots[1].key == key3);
    assert(index.slots[1].block == block_ptr3);
    assert(index.slots[1].state == OCCUPIED);

    assert(hi_destroy(&index) == 0);
    assert(block_pool_free(&pool, block_ptr1) == 0);
    assert(block_pool_free(&pool, block_ptr2) == 0);
    assert(block_pool_free(&pool, block_ptr3) == 0);
    assert(block_pool_destroy(&pool) == 0);
}

static void test_hi_destroy(void)
{
    MemoryPool pool;
    HashIndex index;
    uint64_t key = 100;
    size_t capacity = 1000;
    size_t block_size = 64;
    size_t block_count = 16;

    assert(block_pool_init(&pool, block_size, block_count) == 0);

    void *block_ptr = block_pool_alloc(&pool);
    assert(block_ptr != NULL);

    assert(hi_init(&index, capacity) == 0);
    assert(hi_insert(&index, key, block_ptr) == 0);
    assert(hi_size(&index) == 1);

    assert(hi_destroy(&index) == 0);

    assert(index.entry_count == 0);
    assert(index.capacity == 0);
    assert(index.slots == NULL);

    assert(block_pool_free(&pool, block_ptr) == 0);
    assert(block_pool_destroy(&pool) == 0);
}

void run_hash_index_tests(void)
{
    test_hi_init();
    test_hi_invalid_init();
    test_hi_insert_lookup();
    test_hi_multiple_entries();
    test_hi_lookup_missing();
    test_hi_duplicate_rejected();
    test_hi_full();
    test_hi_remove();
    test_hi_lookup_through_tombstone();
    test_hi_tombstone_reused();
    test_hi_probe_wraparound();
    test_hi_destroy();
}
