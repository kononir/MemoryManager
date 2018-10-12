#include "stdio.h"
#include "stdlib.h"
#include "Test.h"
#include "MemoryManager.h"



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

	system("pause");
	
	return 0;
}

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
	VA* block = NULL;
	
	int errCode;
	int n = 20, szPage = 30, szBlock = -1;

	_init(n, szPage);

	errCode = _malloc(block, szBlock);

	if (errCode != INVALID_PARAMETERS) {
		return TEST_NOT_PASSED;
	} else {
		return TEST_PASSED;
	}
}

int test_malloc_without_init(void) {
	VA* block = NULL;
	
	int errCode;
	int szBlock = 20;

	errCode = _malloc(block, szBlock);

	if (errCode != UNKNOW_ERROR) {
		return TEST_NOT_PASSED;
	} else {
		return TEST_PASSED;
	}
}

int _free_mem(physicalMemory* mem) {
	free(mem -> space);
	free(mem -> head);
	free(mem -> tail);
}