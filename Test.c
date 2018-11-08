#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SysTime.h"
#include "Test.h" 


int main()
{	
	int testNumber, passedNumber = 0;
	long long endTime;

	printf("\t\t\t\t Tests:\n");
	
	for (testNumber = 0; testNumber < sizeof(tests) / sizeof(tests[0]); testNumber++) {
		int rezult = (*tests[testNumber])();

		if (rezult == TEST_PASSED) {
			printf("%s passed!\n", names[testNumber]);
			passedNumber++;
		}
	}

	printf("\n%d tests passed!\n", passedNumber);

	stress_test();

	endTime = getSysTimeInMilliseconds();

	printf("\nTime: %Ld milliseconds\n", endTime - beginTime);

	system("pause");
	
	return 0;
}



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

	prepare_full_free();

	return TEST_PASSED;
}



/*int test_read_data_with_cache_miss_functional(void) {
	PA data = "123456";

	int errCode;
	int numberOfBlocks = 6, currBlock = 0;
	int n = 100, szPage = 2, szBlock = 10;
	long offset = 1;
	size_t dataSize = 6;

	_init(n, szPage);

	while (currBlock < numberOfBlocks) {
		VA block = NULL;

		_malloc(&block, szBlock);

		_write(block + 1, data, dataSize);

		errCode = _read(block + 1, data, dataSize);

		currBlock++;
	}

	return TEST_PASSED;
}*/



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
	long minVasSize = 0;
	long maxVasSize = hardSize;

	int curVasSizeIndex = 0;
	int maxVasSizeIndex = 10;

	int vasSizeMas[10] = {1000, 2500, 4096, 7000, 8192, 16384, 25000, 32768, 47500, 65536};

	srand(time(NULL));

	for (curVasSizeIndex; curVasSizeIndex < maxVasSizeIndex; curVasSizeIndex++) {
		int curIteration = 0;
		int maxIteration = 20;

		long curVasSize = vasSizeMas[curVasSizeIndex];

		printf("\t\t\t\tCurrent VAS size: %ld\n", curVasSize);

		for (curIteration; curIteration < maxIteration; curIteration++) {
			int logicTime = 0;
			double takenSpaceShare;
			long avgRegionSize;

			int errCode;

			_init(1, curVasSize);

			new_list();

			do {
				double vasTaken = (double) (hardSize - table.vas.hardFree);
				double vasSize = (double) table.vas.size;
				double limit = 0.8;

				if (vasTaken / vasSize >= limit) {
					delete_random_block();
				} else {
					errCode = add_random_size_block();
				}

				logicTime++;
			} while (errCode == SUCCESSFUL_EXECUTION);

			takenSpaceShare = calculate_taken_space_share();
			avgRegionSize = calculate_free_region_average_size();

			/*
				Здесь можно посчитать какие-нибудь ещё характеристики для дальнейшего их вывода
			*/

			printf("Logic time: %d\t Taken space share: %f\t Free region average size: %ld\n", logicTime, takenSpaceShare, avgRegionSize);

			delete_list();
		}
	}
}

int add_random_size_block() {
	VA blcVirtAdr = NULL;

	long vasFree = table.vas.size - (hardSize - table.vas.hardFree);

	long minSegmSize = 1;
	long maxSegmSize = vasFree < ramSize ? vasFree: ramSize;
	long curSegmSize = rand() % (maxSegmSize - minSegmSize + 1) + minSegmSize;

	int errCode;

	errCode = _malloc(&blcVirtAdr, curSegmSize);

	add_block_to_list(blcVirtAdr);

	return errCode;
}

void delete_random_block() {
	block* blc = NULL;

	int minBlcNum = 0;
	int maxBlcNum = blcList.length - 1;
	int curBlcNum = rand() % (maxBlcNum - minBlcNum + 1) + minBlcNum;

	find_block_in_list_by_number(&blc, curBlcNum);

	_free(blc -> virtAdr);

	delete_block_from_list(&blc);
}

void new_list() {
	blcList.length = 0;
	blcList.head = NULL;
	blcList.tail = NULL;
}

void delete_list() {
	block* blc = blcList.head;

	while (blc != NULL) {
		block* nextBlc = blc -> next;

		delete_block_from_list(&blc);

		blc = nextBlc;
	}

	blcList.length = 0;
}

void add_block_to_list(VA blcVirtAdr) {
	block* blc = (block*) malloc(sizeof(block));

	blc -> virtAdr = blcVirtAdr;

	if (blcList.head == NULL && blcList.tail == NULL) {
		blc -> next = NULL;
		blc -> prev = NULL;

		blcList.head = blc;
		blcList.tail = blc;
	} else {
		blc -> prev = blcList.tail;
		blc -> next = NULL;

		blcList.tail -> next = blc;

		blcList.tail = blc;
	}

	blcList.length++;
}

void delete_block_from_list(block** blc) {
	block* prevBlc = (*blc) -> prev;
	block* nextBlc = (*blc) -> next;
	
	if (prevBlc != NULL && nextBlc != NULL) {
		prevBlc -> next = nextBlc;
		nextBlc -> prev = prevBlc;
	} else if (prevBlc == NULL && nextBlc == NULL) {
		blcList.head = NULL;
		blcList.tail = NULL;
	} else if (prevBlc == NULL) {
		nextBlc -> prev = NULL;
		blcList.head = nextBlc;
	} else if (nextBlc == NULL) {
		prevBlc -> next = NULL;
		blcList.tail = prevBlc;
	}

	free((*blc));

	(*blc) = NULL;

	blcList.length--;
}

void find_block_in_list_by_number(block** blc, int blcNum) {
	int curBlcNum = 0;
	
	(*blc) = blcList.head;

	while (curBlcNum < blcNum) {
		(*blc) = (*blc) -> next;
		curBlcNum++;
	}
}

long calculate_free_region_average_size() {
	segment* curSegm = table.vas.head;

	long vasFree, vasTaken;
	int regionNum = 0;

	if (curSegm -> offset > 0) {
		regionNum++;
	}

	while (curSegm -> next != NULL) {
		segment* nextSegm = curSegm -> next;

		long beginOfSpace = curSegm -> segmentSize + curSegm -> offset;
		long endOfSpace = nextSegm -> offset;
		long space = endOfSpace - beginOfSpace;

		if (space > 0) {
			regionNum++;
		}

		curSegm = nextSegm;
	}

	if (curSegm -> offset + curSegm -> segmentSize < table.vas.size) {
		regionNum++;
	}

	vasTaken = hardSize - table.vas.hardFree;
	vasFree = table.vas.size - vasTaken;

	return vasFree / regionNum;
}

double calculate_taken_space_share() {
	double vasTaken = (double) (hardSize - table.vas.hardFree);
	double vasSize = (double) table.vas.size;

	return vasTaken / vasSize;
}