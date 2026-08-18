#include <assert.h>

#include "core/hash_index.h"

static void test_hi_init(void) {}

static void test_hi_invalid_init(void) {}

static void test_hi_insert_lookup(void) {}

static void test_hi_multiple_entries(void) {}

static void test_hi_lookup_missing(void) {}

static void test_hi_duplicate_rejected(void) {}

static void test_hi_full(void) {}

static void test_hi_remove(void) {}

static void test_hi_lookup_through_tombstone(void) {}

static void test_hi_tombstone_reused(void) {}

static void test_hi_probe_wraparound(void) {}

static void test_hi_destroy(void) {}

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
