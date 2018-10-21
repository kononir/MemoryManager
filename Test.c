#include <time.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "Test.h"
#include "MemoryManager.h"


/*#define SIZE 50000

struct list
{
	struct list* next;
	struct list* priveos;
	VA va;
	int size;
} *beginn = NULL;

void stressTesting();
void add(VA va, int size);
void del(struct list* list);
struct list* get(int number);
void initial();*/



int main()
{	
	int testNumber, passedNumber = 0;

	printf("\t\t\t\t   Testing: \n");
	
	for (testNumber = 0; testNumber < sizeof(tests) / sizeof(tests[0]); testNumber++) {
		int rezult = (*tests[testNumber])();

		if (rezult == TEST_PASSED) {
			printf("%s passed!\n", names[testNumber]);
			passedNumber++;
		}
	}

	//stressTesting();

	printf("\n%d tests passed!\n", passedNumber);

	system("pause");
	
	return 0;
}

/*void stressTesting()
{
	int res = 0;
	int size = 0;
	int listSize = 0;
	int tsize = 0;
	int iterations = 0;
	int iter;
	VA va = NULL;
	struct list *list = NULL;

	srand(time(0));

	for (iter = 0; iter < 10; iter++)
	{
		initial();
		_init(1, SIZE);
		res = 0;
		size = 0;
		listSize = 0;
		iterations = 0;
		do
		{
			if (size < SIZE * 0.7)
			{
				tsize = rand() % (int)((SIZE - size)*0.9);
				res = _malloc(&va, tsize);
				if (res == 0)
				{
					iterations++;
					add(va, tsize);
					listSize++;
					size += tsize;
				}
				va = NULL;
			}
			else
			{
				list = get(rand() % listSize);
				res = _free(list->va);
				listSize--;
				size -= list->size;
				del(list);
			}
		} while (res == 0);
		printf("\tAlloctated size: %d\tFree space size: %d\tTried to malloc size: %d\tIterations maked: %d\n", size, SIZE - size, tsize, iterations);
	}
}

void add(VA va, int size)
{
	if (beginn->va < 0) beginn->va = va;
	else
	{
		struct list *newOne = (struct list*) malloc(sizeof(struct list));
		newOne->priveos = NULL;
		newOne->next = beginn;
		beginn->priveos = newOne;
		newOne->va = va;
		newOne->size = size;
		beginn = newOne;
	}
}

void del(struct list* list)
{
	if (list == beginn)
	{
		if (list->next != NULL) list->next->priveos = NULL;
		else
		{
			list->va = (char*) - 1;
		}
		beginn = list->next;
		free(list);
	}
	else
	{
		list->priveos->next = list->next;
		if (list->next != NULL)	list->next->priveos = list->priveos;
		free(list);
	}
}

struct list* get(int number)
{
	struct list* list = beginn;
	int i;
	for (i = 0; i < number; i++)
	{
		list = list->next;
	}
	return list;
}

void initial()
{
	while (beginn != NULL) del(beginn);
	beginn = (struct list*) malloc(sizeof(struct list));
	beginn->next = NULL;
	beginn->priveos = NULL;
	beginn->va = (char*) - 1;
}*/

int test_init_invalid_parameters(void) {
	int n = -5, szPage = -4;

	int errCode = _init(n, szPage);
	assert(errCode == INVALID_PARAMETERS);

	return TEST_PASSED;
}

int test_init_successful_execution(void) {
	int n = 20, szPage = 30;

	int errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);

	return TEST_PASSED;
}

int test_malloc_invalid_parameters(void) {
	VA block = NULL;
	
	int errCode;
	int n = 20, szPage = 30, szBlock = -1;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _malloc(&block, szBlock);
	assert(errCode == INVALID_PARAMETERS);

	return TEST_PASSED;
}

int test_malloc_without_init(void) {
	VA block = NULL;
	
	int errCode;
	int szBlock = 20;

	errCode = _destroy_virtual_address_space();
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _malloc(&block, szBlock);
	assert(errCode == UNKNOW_ERROR);

	return TEST_PASSED;
}

int test_malloc_one_block(void) {
	VA block = NULL;

	int errCode;
	int n = 20, szPage = 30, szBlock = 20;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _malloc(&block, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);

	return TEST_PASSED;
}

int test_malloc_two_blocks(void) {
	VA block1 = NULL;
	VA block2 = NULL;

	int errCode;
	int n = 20, szPage = 30, szBlock = 20;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block1, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _malloc(&block2, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);

	return TEST_PASSED;
}

int test_malloc_three_blocks(void) {
	VA block1 = NULL;
	VA block2 = NULL;
	VA block3 = NULL;

	int errCode;
	int n = 20, szPage = 30, szBlock1And2 = 20, szBlock3 = 15;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block1, szBlock1And2);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block2, szBlock1And2);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _free(block1);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _malloc(&block3, szBlock3);
	assert(errCode == SUCCESSFUL_EXECUTION);

	return TEST_PASSED;
}

int test_malloc_four_blocks(void) {
	VA block1 = NULL;
	VA block2 = NULL;
	VA block3 = NULL;
	VA block4 = NULL;

	int errCode;
	int n = 20, szPage = 30, szBlock12AND3 = 20, szBlock4 = 15;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block1, szBlock12AND3);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block2, szBlock12AND3);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block3, szBlock12AND3);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _free(block2);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _malloc(&block4, szBlock4);
	assert(errCode == SUCCESSFUL_EXECUTION);

	return TEST_PASSED;
}

int test_malloc_two_blocks_with_hard(void) {
	VA block1 = NULL;
	VA block2 = NULL;

	int errCode;
	int n = 15, szPage = 2, szBlock = 20;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block1, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _malloc(&block2, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);

	return TEST_PASSED;
}

int test_malloc_ram_out_of_memory(void) {
	VA block = NULL;

	int errCode;
	int n = 9, szPage = 2, szBlock = 30;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _malloc(&block, szBlock);
	assert(errCode == OUT_OF_MEMORY);

	return TEST_PASSED;
}

int test_malloc_hard_out_of_memory(void) {
	VA block1 = NULL;
	VA block2 = NULL;
	int errCode;
	int n = 10000, szPage = 10;
	int szBlock = 100000;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block1, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _malloc(&block2, szBlock);
	assert(errCode == OUT_OF_MEMORY);

	return TEST_PASSED;
}

int test_free_invalid_parameters(void) {
	VA block;

	int errCode;
	int n = 20, szPage = 30;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	block = NULL;

	errCode = _free(block);
	assert(errCode == INVALID_PARAMETERS);

	return TEST_PASSED;
}

int test_free_only_one_block(void) {
	VA block = NULL;

	int errCode;
	int n = 20, szPage = 30, szBlock = 30;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _free(block);
	assert(errCode == SUCCESSFUL_EXECUTION);

	return TEST_PASSED;
}

int test_free_head(void) {
	VA block1 = NULL;
	VA block2 = NULL;

	int errCode;
	int n = 20, szPage = 30, szBlock = 30;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block1, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block2, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _free(block1);
	assert(errCode == SUCCESSFUL_EXECUTION);

	return TEST_PASSED;
}

int test_free_tail(void) {
	VA block1 = NULL;
	VA block2 = NULL;

	int errCode;
	int n = 20, szPage = 30, szBlock = 30;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block1, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block2, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _free(block2);
	assert(errCode == SUCCESSFUL_EXECUTION);

	return TEST_PASSED;
}

int test_free_middle(void) {
	VA block1 = NULL;
	VA block2 = NULL;
	VA block3 = NULL;

	int errCode;
	int n = 20, szPage = 30, szBlock = 30;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block1, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block2, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block3, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _free(block2);
	assert(errCode == SUCCESSFUL_EXECUTION);

	return TEST_PASSED;
}

int test_write_without_malloc(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 30, szBlock = 20;
	size_t dataSize = 6;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _write(block, data, dataSize);
	assert(errCode == INVALID_PARAMETERS);

	return TEST_PASSED;
}

int test_write_out_of_vas_bounds(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 30, szBlock = 20;
	size_t dataSize = 6;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _write(block - 1, data, dataSize);
	assert(errCode == INVALID_PARAMETERS);

	return TEST_PASSED;
}

int test_write_out_of_block_range(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 30, szBlock = 10;
	size_t dataSize = 6;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _write(block + 6, data, dataSize);
	assert(errCode == OUT_OF_BLOCK_RANGE);

	return TEST_PASSED;
}

int test_write_data_without_loading(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 30, szBlock = 20;
	size_t dataSize = 6;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _write(block + 1, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION);

	return TEST_PASSED;
}

int test_write_data_with_loading_one_block_to_free_space(void) {
	VA block1 = NULL;
	VA block2 = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 3, szBlock = 40;
	size_t dataSize = 6;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block1, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block2, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _free(block1);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _write(block2, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION);

	return TEST_PASSED;
}

int test_write_data_with_uploading_one_block(void) {
	VA block1 = NULL;
	VA block2 = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 3, szBlock = 40;
	size_t dataSize = 6;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block1, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block2, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _write(block2, data, dataSize);

	assert(errCode == SUCCESSFUL_EXECUTION);

	return TEST_PASSED;
}

int test_write_data_with_uploading_many_blocks(void) {
	VA block1 = NULL;
	VA block2 = NULL;
	VA block3 = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 2, szBlock1And2 = 20, szBlock3 = 40;
	size_t dataSize = 6;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block1, szBlock1And2);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block2, szBlock1And2);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block3, szBlock3);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _write(block3, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION);

	return TEST_PASSED;
}

int test_write_data_with_cache_miss(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 2, szBlock = 10;
	size_t dataSize = 6;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _write(block, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION);

	return TEST_PASSED;
}

int test_write_data_with_cache_hit(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 2, szBlock = 10;
	size_t dataSize = 6;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _write(block, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _write(block + 1, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION);

	return TEST_PASSED;
}

int test_read_without_malloc(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 30, szBlock = 20;
	size_t dataSize = 6;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _read(block, data, dataSize);
	assert(errCode == INVALID_PARAMETERS);

	return TEST_PASSED;
}

int test_read_out_of_vas_bounds(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 30, szBlock = 20;
	size_t dataSize = 6;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _write(block, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _read(block - 1, data, dataSize);
	assert(errCode == INVALID_PARAMETERS);

	return TEST_PASSED;
}

int test_read_out_of_block_range(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 30, szBlock = 10;
	size_t dataSize = 6;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _write(block, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _read(block + 6, data, dataSize);
	assert(errCode == OUT_OF_BLOCK_RANGE);

	return TEST_PASSED;
}

int test_read_data_without_loading(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 30, szBlock = 20;
	size_t dataSize = 6;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _write(block + 1, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _read(block + 1, data, dataSize);

	assert(errCode == SUCCESSFUL_EXECUTION && strcmp("123456", data) == 0);

	return TEST_PASSED;
}

int test_read_data_with_loading_one_block_to_free_space(void) {
	VA block1 = NULL;
	VA block2 = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 3, szBlock = 40;
	size_t dataSize = 6;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block1, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block2, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _free(block1);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _write(block2, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _read(block2, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION && strcmp("123456", data) == 0);

	return TEST_PASSED;
}

int test_read_data_with_uploading_one_block(void) {
	VA block1 = NULL;
	VA block2 = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 3, szBlock = 40;
	size_t dataSize = 6;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block1, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block2, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _write(block2, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _read(block2, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION && strcmp("123456", data) == 0);

	return TEST_PASSED;
}

int test_read_data_with_uploading_many_blocks(void) {
	VA block1 = NULL;
	VA block2 = NULL;
	VA block3 = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 2, szBlock1And2 = 20, szBlock3 = 40;
	size_t dataSize = 6;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block1, szBlock1And2);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block2, szBlock1And2);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block3, szBlock3);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _write(block3, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _read(block3, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION && strcmp("123456", data) == 0);

	return TEST_PASSED;
}

int test_read_data_with_cache_miss(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 2, szBlock = 10;
	size_t dataSize = 6;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _write(block, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _read(block, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION && strcmp("123456", data) == 0);

	return TEST_PASSED;
}

int test_read_data_with_cache_hit(void) {
	VA block = NULL;
	PA data1 = "1234567890";
	PA data2 = "999999999";

	int errCode;
	int n = 20, szPage = 2, szBlock = 10;
	size_t dataSize1 = 10;
	size_t dataSize2 = 9;

	errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _malloc(&block, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _write(block, data1, dataSize1);
	assert(errCode == SUCCESSFUL_EXECUTION);
	errCode = _write(block + 1, data2, dataSize2);
	assert(errCode == SUCCESSFUL_EXECUTION);

	errCode = _read(block, data1, dataSize1);
	assert(errCode == SUCCESSFUL_EXECUTION && strcmp("1999999999", data1) == 0);

	return TEST_PASSED;
}