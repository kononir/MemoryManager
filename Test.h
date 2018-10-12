#ifndef TEST_H

#define TEST_PASSED 0
#define TEST_NOT_PASSED 1

int test_init_invalid_parameters(void), 
	test_init_successful_execution(void), 
	test_malloc_invalid_parameters(void),
	test_malloc_without_init(void);

int (*tests[])(void) = {
	test_init_invalid_parameters,
	test_init_successful_execution,
	test_malloc_invalid_parameters,
	test_malloc_without_init
};

#endif TEST_H