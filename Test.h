#include "MemoryManager.h"

#ifndef TEST_H

#define TEST_PASSED 0
#define TEST_NOT_PASSED 1

void prepare_vas_initialization(int n, int szPage);
void prepare_segment_memory_allocation(VA* ptr, long szBlock);
void prepare_table_cell_memory_allocation(long szBlock);
void prepare_hard_segment_memory_allocation(long szBlock);
void prepare_cash_initialization();
void prepare_table_cell_write(size_t szBlock, PA data, long szBuffer, long offset);
void prepare_hard_segment_write(size_t szBlock, PA data, long szBuffer, long offset);
void prepare_cash_record_write(PA data, long szBuffer, long offset);
void prepare_vas_space_free();
void prepare_segments_free();
void prepare_table_cells_free();
void prepare_hard_drive_free();
void prepare_cash_free();
void prepare_full_free();
void prepare_first_segment_free();
void prepare_first_table_cell_free();
void prepare_first_hard_segment_free();

int test_init_invalid_number_of_pages(void),
	test_init_invalid_page_size(void),
	test_init_invalid_virtual_address_space_size(void),
	test_init_invalid_page_size(void),
	test_init_successful_execution(void),
	test_malloc_invalid_block_size(void),
	test_malloc_without_init(void),
	test_malloc_ram_out_of_memory(void),
	test_malloc_successful_execution(void),
	test_free_invalid_virtual_address(void),
	test_free_only_one_block(void),
	test_free_with_cash(void),
	test_write_invalid_virtual_address(void),
	test_write_out_of_left_virtual_address_space_bound(void),
	test_write_out_of_right_virtual_address_space_bound(void),
	test_write_out_of_block_range(void),
	test_write_data_with_loading_one_block_to_free_space(void),
	test_write_data_without_loading(void),
	test_write_data_with_cache_miss(void),
	test_write_data_with_cache_hit(void),
	test_read_invalid_virtual_address(void),
	test_read_out_of_left_virtual_address_space_bound(void),
	test_read_out_of_right_virtual_address_space_bound(void),
	test_read_out_of_block_range(void),
	test_read_data_with_loading_one_block_to_free_space(void),
	test_read_data_without_loading(void),
	test_read_data_with_cache_miss(void),
	test_read_data_with_cache_hit(void);

int (*tests[])(void) = {
	test_init_invalid_number_of_pages,
	test_init_invalid_page_size,
	test_init_invalid_virtual_address_space_size,
	test_init_successful_execution,
	test_malloc_invalid_block_size,
	test_malloc_without_init,
	test_malloc_ram_out_of_memory,
	test_malloc_successful_execution,
	test_free_invalid_virtual_address,
	test_free_only_one_block,
	test_free_with_cash,
	test_write_invalid_virtual_address,
	test_write_out_of_left_virtual_address_space_bound,
	test_write_out_of_right_virtual_address_space_bound,
	test_write_out_of_block_range,
	test_write_data_with_loading_one_block_to_free_space,
	test_write_data_without_loading,
	test_write_data_with_cache_miss,
	test_write_data_with_cache_hit,
	test_read_invalid_virtual_address,
	test_read_out_of_left_virtual_address_space_bound,
	test_read_out_of_right_virtual_address_space_bound,
	test_read_out_of_block_range,
	test_read_data_with_loading_one_block_to_free_space,
	test_read_data_without_loading,
	test_read_data_with_cache_miss,
	test_read_data_with_cache_hit
};

char* names[] = {
	"test_init_invalid_number_of_pages",
	"test_init_invalid_page_size",
	"test_init_invalid_virtual_address_space_size",
	"test_init_successful_execution",
	"test_malloc_invalid_block_size",
	"test_malloc_without_init",
	"test_malloc_ram_out_of_memory",
	"test_malloc_successful_execution",
	"test_free_invalid_virtual_address",
	"test_free_only_one_block",
	"test_free_with_cash",
	"test_write_invalid_virtual_address",
	"test_write_out_of_left_virtual_address_space_bound",
	"test_write_out_of_right_virtual_address_space_bound",
	"test_write_out_of_block_range",
	"test_write_data_with_loading_one_block_to_free_space",
	"test_write_data_without_loading",
	"test_write_data_with_cache_miss",
	"test_write_data_with_cache_hit",
	"test_read_invalid_virtual_address",
	"test_read_out_of_vas_bounds",
	"test_read_out_of_left_virtual_address_space_bound",
	"test_read_out_of_right_virtual_address_space_bound",
	"test_read_out_of_block_range",
	"test_read_data_with_loading_one_block_to_free_space",
	"test_read_data_without_loading",
	"test_read_data_with_cache_miss",
	"test_read_data_with_cache_hit"
};

#endif TEST_H