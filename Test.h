#ifndef TEST_H

#define TEST_PASSED 0
#define TEST_NOT_PASSED 1

int test_init_invalid_parameters(void), 
	test_init_successful_execution(void), 
	test_malloc_invalid_parameters(void),
	test_malloc_without_init(void),
	test_malloc_one_block(void),
	test_malloc_two_blocks(void),
	test_malloc_three_blocks(void),
	test_malloc_four_blocks(void),
	test_malloc_one_block_out_of_memory(void),
	test_malloc_two_blocks_out_of_memory(void),
	test_malloc_three_blocks_out_of_memory(void),
	test_malloc_four_blocks_out_of_memory(void),
	test_free_invalid_parameters(void),
	test_free_only_one_block(void),
	test_free_head(void),
	test_free_tail(void),
	test_free_middle(void);

int (*tests[])(void) = {
	test_init_invalid_parameters,
	test_init_successful_execution,
	test_malloc_invalid_parameters,
	test_malloc_without_init,
	test_malloc_one_block,
	test_malloc_two_blocks,
	test_malloc_three_blocks,
	test_malloc_four_blocks,
	test_malloc_one_block_out_of_memory,
	test_malloc_two_blocks_out_of_memory,
	test_malloc_three_blocks_out_of_memory,
	test_malloc_four_blocks_out_of_memory,
	test_free_invalid_parameters,
	test_free_only_one_block,
	test_free_head,
	test_free_tail,
	test_free_middle
};

#endif TEST_H