#include <time.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "Test.h"


/*#define SIZE 65536

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

int test_init_invalid_number_of_pages(void) {
	int n = -5, szPage = 3;

	int errCode = _init(n, szPage);
	assert(errCode == INVALID_PARAMETERS);

	return TEST_PASSED;
}

int test_init_invalid_page_size(void) {
	int n = 3, szPage = -4;

	int errCode = _init(n, szPage);
	assert(errCode == INVALID_PARAMETERS);

	return TEST_PASSED;
}

int test_init_invalid_virtual_address_space_size(void) {
	int n = 10000, szPage = 10;

	int errCode = _init(n, szPage);
	assert(errCode == INVALID_PARAMETERS);

	return TEST_PASSED;
}

int test_init_successful_execution(void) {
	int n = 20, szPage = 30;

	int errCode = _init(n, szPage);
	assert(errCode == SUCCESSFUL_EXECUTION);

	prepare_vas_space_free();

	return TEST_PASSED;
}

int test_malloc_invalid_block_size(void) {
	VA block = NULL;
	
	int errCode;
	int szBlock = -1;

	errCode = _malloc(&block, szBlock);
	assert(errCode == INVALID_PARAMETERS);

	return TEST_PASSED;
}

int test_malloc_without_init(void) {
	VA block = NULL;
	
	int errCode;
	int szBlock = 20;

	errCode = _malloc(&block, szBlock);
	assert(errCode == UNKNOW_ERROR);

	return TEST_PASSED;
}

int test_malloc_ram_out_of_memory(void) {
	VA block = NULL;

	int errCode;
	int n = 9, szPage = 2, szBlock = ramSize + 1;

	prepare_vas_initialization(n, szPage);

	errCode = _malloc(&block, szBlock);
	assert(errCode == OUT_OF_MEMORY);

	prepare_vas_space_free();

	return TEST_PASSED;
}

int test_malloc_virtual_address_space_out_of_memory(void) {
	VA block = NULL;

	int errCode;
	int n = 9, szPage = 2, szBlock = (n * szPage) + 1;

	prepare_vas_initialization(n, szPage);

	errCode = _malloc(&block, szBlock);
	assert(errCode == OUT_OF_MEMORY);

	prepare_vas_space_free();

	return TEST_PASSED;
}

int test_malloc_successful_execution(void) {
	VA block = NULL;

	int errCode;
	int n = 20, szPage = 30, szBlock = 20;

	prepare_vas_initialization(n, szPage);

	errCode = _malloc(&block, szBlock);
	assert(errCode == SUCCESSFUL_EXECUTION);

	prepare_table_cells_free();
	prepare_segments_free();
	prepare_vas_space_free();
	prepare_hard_drive_free();

	return TEST_PASSED;
}

int test_free_invalid_virtual_address(void) {
	VA block = NULL;

	int errCode;
	int n = 20, szPage = 30;

	errCode = _free(block);
	assert(errCode == INVALID_PARAMETERS);

	return TEST_PASSED;
}

int test_free_only_one_block(void) {
	VA block = NULL;

	int errCode;
	int n = 20, szPage = 30, szBlock = 30;

	prepare_vas_initialization(n, szPage);
	prepare_table_cell_memory_allocation(szBlock);
	prepare_segment_memory_allocation(&block, szBlock);
	prepare_hard_segment_memory_allocation(szBlock);

	errCode = _free(table.vas.space);
	assert(errCode == SUCCESSFUL_EXECUTION);

	prepare_vas_space_free();

	return TEST_PASSED;
}

int test_free_with_cash(void) {
	PA data = "123456";
	VA block = NULL;

	int errCode;
	int n = 20, szPage = 30;
	long szBlock = maxRecordSize;
	long szBuffer = 6;
	long offset = 0;

	prepare_vas_initialization(n, szPage);
	prepare_table_cell_write(szBlock, data, szBuffer, offset);
	prepare_segment_memory_allocation(&block, szBlock);
	prepare_hard_segment_write(szBlock, data, szBuffer, offset);
	prepare_cash_record_write(data, szBuffer, offset);

	errCode = _free(block);
	assert(errCode == SUCCESSFUL_EXECUTION);

	prepare_vas_space_free();
	prepare_cash_free();

	return TEST_PASSED;
}

int test_write_invalid_virtual_address(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 30, szBlock = 20;
	size_t dataSize = 6;

	errCode = _write(block, data, dataSize);
	assert(errCode == INVALID_PARAMETERS);

	return TEST_PASSED;
}

int test_write_out_of_left_virtual_address_space_bound(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 30, szBlock = 20;
	size_t dataSize = 6;

	prepare_vas_initialization(n, szPage);
	block = table.vas.space;

	errCode = _write(block - 1, data, dataSize);
	assert(errCode == INVALID_PARAMETERS);

	prepare_vas_space_free();

	return TEST_PASSED;
}

int test_write_out_of_right_virtual_address_space_bound(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 30, szBlock = 20;
	size_t dataSize = 6;

	prepare_vas_initialization(n, szPage);
	block = table.vas.space;

	errCode = _write(block + (n * szPage), data, dataSize);
	assert(errCode == INVALID_PARAMETERS);

	prepare_vas_space_free();

	return TEST_PASSED;
}

int test_write_out_of_block_range(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 30, szBlock = 10;
	int segmOffset = 0;
	size_t dataSize = 6;

	prepare_vas_initialization(n, szPage);
	prepare_segment_memory_allocation(&block, szBlock);

	errCode = _write(block + 6, data, dataSize);
	assert(errCode == OUT_OF_BLOCK_RANGE);

	prepare_segments_free();
	prepare_vas_space_free();

	return TEST_PASSED;
}

int test_write_data_with_loading_one_block_to_free_space(void) {
	VA block1 = NULL;
	VA block2 = NULL;
	PA data = "123456";

	int errCode;
	int n = 2, szPage = ramSize, szBlock = ramSize;
	size_t dataSize = 6;

	prepare_vas_initialization(n, szPage);
	prepare_cash_initialization();

	prepare_table_cell_memory_allocation(szBlock);
	prepare_segment_memory_allocation(&block1, szBlock);
	prepare_hard_segment_memory_allocation(szBlock);
	
	curSegmentNumber = 1;
	prepare_table_cell_memory_allocation(szBlock);
	prepare_segment_memory_allocation(&block2, szBlock);
	prepare_hard_segment_memory_allocation(szBlock);

	prepare_first_segment_free();
	prepare_first_table_cell_free();
	prepare_first_hard_segment_free();

	errCode = _write(block2, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION);

	prepare_full_free();

	return TEST_PASSED;
}

int test_write_data_without_loading(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 30, szBlock = 20;
	size_t dataSize = 6;

	prepare_vas_initialization(n, szPage);
	prepare_cash_initialization();

	prepare_table_cell_memory_allocation(szBlock);
	prepare_segment_memory_allocation(&block, szBlock);
	prepare_hard_segment_memory_allocation(szBlock);

	errCode = _write(block + 1, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION);

	prepare_full_free();

	return TEST_PASSED;
}

int test_write_data_with_cache_miss(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 2, szBlock = 10;
	size_t dataSize = 6;

	prepare_vas_initialization(n, szPage);
	prepare_cash_initialization();

	prepare_table_cell_memory_allocation(szBlock);
	prepare_segment_memory_allocation(&block, szBlock);
	prepare_hard_segment_memory_allocation(szBlock);

	errCode = _write(block, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION);

	prepare_full_free();

	return TEST_PASSED;
}

int test_write_data_with_cache_hit(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 2, szBlock = 10;
	long offset = 0;
	size_t dataSize = 6;

	prepare_vas_initialization(n, szPage);

	prepare_table_cell_write(szBlock, data, dataSize, offset);
	prepare_segment_memory_allocation(&block, szBlock);
	prepare_hard_segment_write(szBlock, data, dataSize, offset);
	prepare_cash_record_write(data, dataSize, offset);

	errCode = _write(block + 1, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION);

	prepare_full_free();

	return TEST_PASSED;
}

int test_read_invalid_virtual_address(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 30, szBlock = 20;
	size_t dataSize = 6;

	errCode = _read(block, data, dataSize);
	assert(errCode == INVALID_PARAMETERS);

	return TEST_PASSED;
}

int test_read_out_of_left_virtual_address_space_bound(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 30, szBlock = 20;
	size_t dataSize = 6;

	prepare_vas_initialization(n, szPage);
	block = table.vas.space;

	errCode = _read(block - 1, data, dataSize);
	assert(errCode == INVALID_PARAMETERS);

	return TEST_PASSED;
}

int test_read_out_of_right_virtual_address_space_bound(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 30, szBlock = 20;
	size_t dataSize = 6;

	prepare_vas_initialization(n, szPage);
	block = table.vas.space;

	errCode = _read(block + (n * szPage), data, dataSize);
	assert(errCode == INVALID_PARAMETERS);

	return TEST_PASSED;
}

int test_read_out_of_block_range(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 30, szBlock = 10;
	size_t dataSize = 6;

	prepare_vas_initialization(n, szPage);
	prepare_segment_memory_allocation(&block, szBlock);

	errCode = _read(block + 6, data, dataSize);
	assert(errCode == OUT_OF_BLOCK_RANGE);

	return TEST_PASSED;
}

int test_read_data_with_loading_one_block_to_free_space(void) {
	VA block1 = NULL;
	VA block2 = NULL;
	PA data = "123456";

	int errCode;
	int n = 2, szPage = ramSize, szBlock = ramSize + 1;
	long offset = 0;
	size_t dataSize = 6;

	prepare_vas_initialization(n, szPage);
	prepare_cash_initialization();

	prepare_table_cell_memory_allocation(szBlock);
	prepare_segment_memory_allocation(&block1, szBlock);
	prepare_hard_segment_memory_allocation(szBlock);
	
	curSegmentNumber = 1;
	prepare_table_cell_write(szBlock, data, dataSize, offset);
	prepare_segment_memory_allocation(&block2, szBlock);
	prepare_hard_segment_write(szBlock, data, dataSize, offset);

	prepare_first_segment_free();
	prepare_first_table_cell_free();
	prepare_first_hard_segment_free();

	errCode = _read(block2, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION && strcmp("123456", data) == 0);

	prepare_full_free();

	return TEST_PASSED;
}

int test_read_data_without_loading(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 30, szBlock = 20;
	int offset = 1;
	size_t dataSize = 6;

	prepare_vas_initialization(n, szPage);
	prepare_cash_initialization();

	prepare_table_cell_write(szBlock, data, dataSize, offset);
	prepare_segment_memory_allocation(&block, szBlock);
	prepare_hard_segment_write(szBlock, data, dataSize, offset);

	errCode = _read(block + 1, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION && strcmp("123456", data) == 0);

	prepare_full_free();

	return TEST_PASSED;
}

int test_read_data_with_cache_miss(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 2, szBlock = 10;
	long offset = 1;
	size_t dataSize = 6;

	prepare_vas_initialization(n, szPage);
	prepare_cash_initialization();

	prepare_table_cell_write(szBlock, data, dataSize, offset);
	prepare_segment_memory_allocation(&block, szBlock);
	prepare_hard_segment_write(szBlock, data, dataSize, offset);

	errCode = _read(block + 1, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION && strcmp("123456", data) == 0);

	prepare_full_free();

	return TEST_PASSED;
}

int test_read_data_with_cache_hit(void) {
	VA block = NULL;
	PA data = "123456";

	int errCode;
	int n = 20, szPage = 2, szBlock = 10;
	long offset = 1;
	size_t dataSize = 6;

	prepare_vas_initialization(n, szPage);

	prepare_table_cell_write(szBlock, data, dataSize, offset);
	prepare_segment_memory_allocation(&block, szBlock);
	prepare_hard_segment_write(szBlock, data, dataSize, offset);
	prepare_cash_record_write(data, dataSize, offset);

	errCode = _read(block + 1, data, dataSize);
	assert(errCode == SUCCESSFUL_EXECUTION && strcmp("123456", data) == 0);

	return TEST_PASSED;
}



void prepare_vas_initialization(int n, int szPage) {
	table.vas.space = (VA) malloc(n * szPage);
	table.vas.size = n * szPage;
	table.vas.ramFree = ramSize;
	table.vas.hardFree = hardSize;
}

void prepare_segment_memory_allocation(VA* ptr, long szBlock) {
	segment* segm = (segment*) malloc(sizeof(segment));

	segm -> segmentNumber = curSegmentNumber;
	segm -> segmentSize = szBlock;

	if (table.vas.head == NULL) {
		segm -> next = NULL;
		segm -> prev = NULL;

		segm -> offset = 0;
		segm -> virtAddr = table.vas.space;

		table.vas.head = segm;
		table.vas.tail = segm;
	} else {
		segm -> prev = table.vas.tail;
		segm -> next = NULL;

		segm -> offset = table.vas.tail -> segmentSize + table.vas.tail -> offset;
		segm -> virtAddr = table.vas.space + segm -> offset;

		table.vas.tail -> next = segm;

		table.vas.tail = segm;
	}

	(*ptr) = segm -> virtAddr;
}

void prepare_table_cell_memory_allocation(long szBlock) {
	tableCell* tc = (tableCell*) malloc(sizeof(tableCell));

	tc -> modification = 0;
	tc -> segmentSize = szBlock;
	tc -> segmentNumber = curSegmentNumber;

	if (table.vas.ramFree < (long) szBlock) {
		tc -> presence = 0;
		
		tc -> physAddr = NULL;
	} else {
		tc -> presence = 1;
		table.vas.ramFree -= (long) szBlock;

		tc -> physAddr = (PA) malloc((long)szBlock);
	}

	if (table.head == NULL) {
		tc -> next = NULL;
		tc -> prev = NULL;

		table.head = tc;
		table.tail = tc;
	} else {
		tc -> prev = table.tail;
		tc -> next = NULL;

		table.tail -> next = tc;

		table.tail = tc;
	}
}

void prepare_hard_segment_memory_allocation(long szBlock) {
	hardSegment* hardSegm = (hardSegment*) malloc(sizeof(hardSegment));

	hardSegm -> data = (PA) malloc((long)szBlock);
	table.vas.hardFree -= (long) szBlock;

	hardSegm -> segmentNumber = curSegmentNumber;
	hardSegm -> segmentSize = (long) szBlock;

	if (drive.head == NULL) {
		hardSegm -> next = NULL;
		hardSegm -> prev = NULL;

		drive.head = hardSegm;
		drive.tail = hardSegm;
	} else {
		hardSegm -> prev = drive.tail;
		hardSegm -> next = NULL;

		drive.tail -> next = hardSegm;

		drive.tail = hardSegm;
	}
}

void prepare_cash_initialization() {
	int recNum = 0;

	for (recNum; recNum < maxRecordNumber; recNum++) {
		cashRecord* rec = (cashRecord*) malloc(sizeof(cashRecord));

		rec -> data = (PA) malloc(maxRecordSize);

		rec -> modification = 0;
		rec -> reality = 0;

		if (csh.head == NULL) {
			rec -> next = NULL;
			rec -> prev = NULL;

			csh.head = rec;
			csh.tail = rec;
		} else {
			rec -> prev = csh.tail;
			rec -> next = NULL;

			csh.tail -> next = rec;

			csh.tail = rec;
		}
	}
}

void prepare_table_cell_write(size_t szBlock, PA data, long szBuffer, long offset) {
	tableCell* tc = (tableCell*) malloc(sizeof(tableCell));

	tc -> modification = 0;
	tc -> segmentSize = szBlock;
	tc -> segmentNumber = curSegmentNumber;

	if (table.vas.ramFree < (long) szBlock) {
		tc -> presence = 0;
		
		tc -> physAddr = NULL;
	} else {
		tc -> presence = 1;
		table.vas.ramFree -= (long) szBlock;

		tc -> physAddr = (PA) malloc((long)szBlock);
		memcpy(tc -> physAddr + offset, data, szBuffer);
	}

	if (table.head == NULL) {
		tc -> next = NULL;
		tc -> prev = NULL;

		table.head = tc;
		table.tail = tc;
	} else {
		tc -> prev = table.tail;
		tc -> next = NULL;

		table.tail -> next = tc;

		table.tail = tc;
	}
}

void prepare_hard_segment_write(size_t szBlock, PA data, long szBuffer, long offset) {
	hardSegment* hardSegm = (hardSegment*) malloc(sizeof(hardSegment));

	hardSegm -> data = (PA) malloc((long)szBlock);
	table.vas.hardFree -= (long) szBlock;

	hardSegm -> segmentNumber = curSegmentNumber;
	hardSegm -> segmentSize = (long) szBlock;

	if (drive.head == NULL) {
		hardSegm -> next = NULL;
		hardSegm -> prev = NULL;

		drive.head = hardSegm;
		drive.tail = hardSegm;
	} else {
		hardSegm -> prev = drive.tail;
		hardSegm -> next = NULL;

		drive.tail -> next = hardSegm;

		drive.tail = hardSegm;
	}

	memcpy(hardSegm -> data + offset, data, szBuffer);
}

void prepare_cash_record_write(PA data, long szBuffer, long offset) {
	cashRecord* rec = (cashRecord*) malloc(sizeof(cashRecord));
	rec -> physAddr = table.head -> physAddr;
	rec -> data = (PA) malloc(maxRecordSize);
	rec -> modification = 0;
	rec -> reality = 1;
	rec -> next = NULL;
	rec -> prev = NULL;

	csh.head = rec;
	csh.tail = rec;

	memcpy(rec -> data + offset, data, szBuffer);
}

void prepare_vas_space_free() {
	free(table.vas.space);
	table.vas.space = NULL;

	curSegmentNumber = 0;
}

void prepare_segments_free() {
	segment* segm = table.vas.head;

	while (segm != NULL) {
		segment* nextSegm = segm -> next;
		free(segm);

		segm = nextSegm;
	}

	table.vas.head = NULL;
	table.vas.tail = NULL;
}

void prepare_table_cells_free() {
	tableCell* tc = table.head;

	while (tc != NULL) {
		tableCell* nextTC = tc -> next;

		if (tc -> presence == 1) {
			free(tc -> physAddr);

			table.vas.ramFree += tc -> segmentSize;
		}
		
		free(tc);

		tc = nextTC;
	}

	table.head = NULL;
	table.tail = NULL;
}

void prepare_hard_drive_free() {
	hardSegment* hardSegm = drive.head;

	while (hardSegm != NULL) {
		hardSegment* nextHardSegm = hardSegm -> next;
		free(hardSegm -> data);

		table.vas.hardFree += hardSegm -> segmentSize;

		free(hardSegm);

		hardSegm = nextHardSegm;
	}

	drive.head = NULL;
	drive.tail = NULL;
}

void prepare_cash_free() {
	cashRecord* rec = csh.head;

	while (rec != NULL) {
		cashRecord* nextRec = rec -> next;
		free(rec -> data);
		free(rec);

		rec = nextRec;
	}

	csh.head = NULL;
	csh.tail = NULL;

	curRecordNumber = 0;
}

void prepare_full_free() {
	prepare_table_cells_free();
	prepare_segments_free();
	prepare_vas_space_free();
	prepare_hard_drive_free();
	prepare_cash_free();
}

void prepare_first_segment_free() {
	free(table.vas.head);
	table.vas.tail -> prev = NULL;

	table.vas.head = table.vas.tail;
}

void prepare_first_table_cell_free() {
	free(table.head -> physAddr);
	free(table.head);
	table.tail -> prev = NULL;

	table.head = table.tail;
}

void prepare_first_hard_segment_free() {
	free(drive.head -> data);
	free(drive.head);
	drive.tail -> prev = NULL;

	drive.head = drive.tail;
}



void stress_test() {
	
}