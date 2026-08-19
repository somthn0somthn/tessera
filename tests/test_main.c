#include <stdio.h>
#include <assert.h>

void run_block_pool_tests(void);
void run_hash_index_tests(void);

int main(void)
{
    run_block_pool_tests();
    puts("All block_pool tests passed.");

    run_hash_index_tests();
    puts("All hash_index tests passed.");

    return 0;
}
