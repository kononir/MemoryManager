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
	test_malloc_two_blocks_with_hard(void),
	test_malloc_ram_out_of_memory(void),
	test_malloc_hard_out_of_memory(void),
	test_free_invalid_parameters(void),
	test_free_only_one_block(void),
	test_free_head(void),
	test_free_tail(void),
	test_free_middle(void),
	test_write_without_malloc(void),
	test_write_out_of_vas_bounds(void),
	test_write_out_of_block_range(void),
	test_write_data_in_block(void);

int (*tests[])(void) = {
	test_init_invalid_parameters,
	test_init_successful_execution,
	test_malloc_invalid_parameters,
	test_malloc_without_init,
	test_malloc_one_block,
	test_malloc_two_blocks,
	test_malloc_three_blocks,
	test_malloc_four_blocks,
	test_malloc_two_blocks_with_hard,
	test_malloc_ram_out_of_memory,
	test_malloc_hard_out_of_memory,
	test_free_invalid_parameters,
	test_free_only_one_block,
	test_free_head,
	test_free_tail,
	test_free_middle,
	test_write_without_malloc,
	test_write_out_of_vas_bounds,
	test_write_out_of_block_range,
	test_write_data_in_block
};

#endif TEST_H