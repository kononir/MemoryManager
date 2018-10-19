#include <time.h>
#include "stdio.h"
#include "stdlib.h"
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
	int testNumber;

	printf("\t\t\t\t   Testing: \n");
	
	for (testNumber = 0; testNumber < sizeof(tests) / sizeof(tests[0]); testNumber++) {
		int rezult = (*tests[testNumber])();
		if (rezult == TEST_PASSED) {
			printf("Test %d - Passed!\n", testNumber);
		} else {
			printf("Test %d - Didn't Passed!\n", testNumber);
		}
	}

	//stressTesting();

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

	if (errCode != INVALID_PARAMETERS) {
		return TEST_NOT_PASSED;
	} else {
		return TEST_PASSED;
	}
}

int test_init_successful_execution(void) {
	int n = 20, szPage = 30;

	int errCode = _init(n, szPage);

	if (errCode != SUCCESSFUL_EXECUTION) {
		return TEST_NOT_PASSED;
	} else {
		return TEST_PASSED;
	}
}

int test_malloc_invalid_parameters(void) {
	VA block = NULL;
	
	int errCode;
	int n = 20, szPage = 30, szBlock = -1;

	_init(n, szPage);

	errCode = _malloc(&block, szBlock);

	if (errCode != INVALID_PARAMETERS) {
		return TEST_NOT_PASSED;
	} else {
		return TEST_PASSED;
	}
}

int test_malloc_without_init(void) {
	VA block = NULL;
	
	int errCode;
	int szBlock = 20;

	_destroy(&vas);

	errCode = _malloc(&block, szBlock);

	if (errCode != UNKNOW_ERROR) {
		return TEST_NOT_PASSED;
	} else {
		return TEST_PASSED;
	}
}

int test_malloc_one_block(void) {
	VA block = NULL;

	int errCode;
	int n = 20, szPage = 30, szBlock = 20;

	_init(n, szPage);

	errCode = _malloc(&block, szBlock);

	if (errCode != SUCCESSFUL_EXECUTION) {
		return TEST_NOT_PASSED;
	} else {
		return TEST_PASSED;
	}
}

int test_malloc_two_blocks(void) {
	VA block1 = NULL;
	VA block2 = NULL;

	int errCode;
	int n = 20, szPage = 30, szBlock = 20;

	_init(n, szPage);

	_malloc(&block1, szBlock);
	errCode = _malloc(&block2, szBlock);

	if (errCode != SUCCESSFUL_EXECUTION) {
		return TEST_NOT_PASSED;
	} else {
		return TEST_PASSED;
	}
}

int test_malloc_three_blocks(void) {
	VA block1 = NULL;
	VA block2 = NULL;
	VA block3 = NULL;

	int errCode;
	int n = 20, szPage = 30, szBlock1And2 = 20, szBlock3 = 15;

	_init(n, szPage);

	_malloc(&block1, szBlock1And2);
	_malloc(&block2, szBlock1And2);
	_free(block1);
	errCode = _malloc(&block3, szBlock3);

	if (errCode != SUCCESSFUL_EXECUTION) {
		return TEST_NOT_PASSED;
	} else {
		return TEST_PASSED;
	}
}

int test_malloc_four_blocks(void) {
	VA block1 = NULL;
	VA block2 = NULL;
	VA block3 = NULL;
	VA block4 = NULL;

	int errCode;
	int n = 20, szPage = 30, szBlock12AND3 = 20, szBlock4 = 15;

	_init(n, szPage);

	_malloc(&block1, szBlock12AND3);
	_malloc(&block2, szBlock12AND3);
	_malloc(&block3, szBlock12AND3);
	_free(block2);
	errCode = _malloc(&block4, szBlock4);

	if (errCode != SUCCESSFUL_EXECUTION) {
		return TEST_NOT_PASSED;
	} else {
		return TEST_PASSED;
	}
}

int test_malloc_two_blocks_with_hard(void) {
	VA block1 = NULL;
	VA block2 = NULL;

	int errCode;
	int n = 15, szPage = 2, szBlock = 20;

	_init(n, szPage);

	_malloc(&block1, szBlock);
	errCode = _malloc(&block2, szBlock);

	if (errCode != SUCCESSFUL_EXECUTION) {
		return TEST_NOT_PASSED;
	} else {
		return TEST_PASSED;
	}
}

int test_malloc_ram_out_of_memory(void) {
	VA block = NULL;

	int errCode;
	int n = 9, szPage = 2, szBlock = 30;

	_init(n, szPage);

	errCode = _malloc(&block, szBlock);

	if (errCode != OUT_OF_MEMORY) {
		return TEST_NOT_PASSED;
	} else {
		return TEST_PASSED;
	}
}

int test_malloc_hard_out_of_memory(void) {
	int errCode;
	int n = 1000, szPage = 10;

	_init(n, szPage);

	do {
		VA block = NULL;
		int szBlock = 10000;

		errCode = _malloc(&block, szBlock);
	} while (errCode != OUT_OF_MEMORY);	

	return TEST_PASSED;
}

int test_free_invalid_parameters(void) {
	VA block;

	int errCode;
	int n = 20, szPage = 30;

	_init(n, szPage);

	block = NULL;
	errCode = _free(block);

	if (errCode != INVALID_PARAMETERS) {
		return TEST_NOT_PASSED;
	} else {
		return TEST_PASSED;
	}
}

int test_free_only_one_block(void) {
	VA block = NULL;

	int errCode;
	int n = 20, szPage = 30, szBlock = 30;

	_init(n, szPage);
	_malloc(&block, szBlock);

	errCode = _free(block);

	if (errCode != SUCCESSFUL_EXECUTION) {
		return TEST_NOT_PASSED;
	} else {
		return TEST_PASSED;
	}
}

int test_free_head(void) {
	VA block1 = NULL;
	VA block2 = NULL;

	int errCode;
	int n = 20, szPage = 30, szBlock = 30;

	_init(n, szPage);
	_malloc(&block1, szBlock);
	_malloc(&block2, szBlock);

	errCode = _free(block1);

	if (errCode != SUCCESSFUL_EXECUTION) {
		return TEST_NOT_PASSED;
	} else {
		return TEST_PASSED;
	}
}

int test_free_tail(void) {
	VA block1 = NULL;
	VA block2 = NULL;

	int errCode;
	int n = 20, szPage = 30, szBlock = 30;

	_init(n, szPage);
	_malloc(&block1, szBlock);
	_malloc(&block2, szBlock);

	errCode = _free(block2);

	if (errCode != SUCCESSFUL_EXECUTION) {
		return TEST_NOT_PASSED;
	} else {
		return TEST_PASSED;
	}
}

int test_free_middle(void) {
	VA block1 = NULL;
	VA block2 = NULL;
	VA block3 = NULL;

	int errCode;
	int n = 20, szPage = 30, szBlock = 30;

	_init(n, szPage);
	_malloc(&block1, szBlock);
	_malloc(&block2, szBlock);
	_malloc(&block3, szBlock);

	errCode = _free(block2);

	if (errCode != SUCCESSFUL_EXECUTION) {
		return TEST_NOT_PASSED;
	} else {
		return TEST_PASSED;
	}
}

int test_write_without_malloc(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 30, szBlock = 20;
	size_t dataSize = 6;

	_init(n, szPage);

	errCode = _write(block, data, dataSize);

	if (errCode != INVALID_PARAMETERS) {
		return TEST_NOT_PASSED;
	} else {
		return TEST_PASSED;
	}
}

int test_write_out_of_vas_bounds(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 30, szBlock = 20;
	size_t dataSize = 6;

	_init(n, szPage);
	_malloc(&block, szBlock);

	errCode = _write(block - 1, data, dataSize);

	if (errCode != INVALID_PARAMETERS) {
		return TEST_NOT_PASSED;
	} else {
		return TEST_PASSED;
	}
}

int test_write_out_of_block_range(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 30, szBlock = 10;
	size_t dataSize = 6;

	_init(n, szPage);
	_malloc(&block, szBlock);

	errCode = _write(block + 6, data, dataSize);

	if (errCode != OUT_OF_BLOCK_RANGE) {
		return TEST_NOT_PASSED;
	} else {
		return TEST_PASSED;
	}
}

int test_write_data_in_block(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 30, szBlock = 20;
	size_t dataSize = 6;

	_init(n, szPage);
	_malloc(&block, szBlock);

	errCode = _write(block, data, dataSize);

	if (errCode != SUCCESSFUL_EXECUTION) {
		return TEST_NOT_PASSED;
	} else {
		return TEST_PASSED;
	}
}