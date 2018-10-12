#include "stdlib.h"
#include "MemoryManager.h"

int _init_physical_memory(int memoryNumber, physicalMemory* mem);
long _take_free_space(VA* ptr, size_t szBlock);
int _find_table_cell_by_segment_number(int segmentNumber, tableCell* tc);
int _add_new_block(segment* prevSegm, tableCell* prevTC, VA* ptr, size_t szBlock);
int _find_last_segment(segment* segm);
int _find_table_cell_by_ptr(tableCell* tc, VA ptr);
int _find_segment_by_segment_number(segment* segm, int segmNumber);
int _free_table_cell(tableCell* tc);
int _free_segment(segment* segm);

int _init (int n, int szPage)
{
	int errCode, ramNumber, hardNumber, cashNumber;

	if (n <= 0 || szPage <= 0) {
		return INVALID_PARAMETERS;
	}

	ramNumber = n * szPage, hardNumber = 65536, cashNumber = 2048;		//errCode - код ошибки

	errCode = _init_physical_memory(ramNumber, &ram);		//инициализируем ОП

	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	errCode = _init_physical_memory(hardNumber, &hard);		//инициализируем память жд

	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	errCode = _init_physical_memory(cashNumber, &cash);		//инициализируем кэш

	return errCode;
}


int _init_physical_memory(int memoryNumber, physicalMemory* mem)
{
	int memorySize = memoryNumber * sizeof(VA);

	if (mem -> space != NULL) {
		return UNKNOW_ERROR;
	}

	mem -> space = (VA*) malloc(memorySize);

	if (mem -> space == NULL){
		return UNKNOW_ERROR;
	} else {
		mem -> head = NULL;
		mem -> tail = NULL;
		mem -> size = memorySize;

		return SUCCESSFUL_EXECUTION;
	}
}



int _malloc (VA* ptr, size_t szBlock)
{
	int errCode;

	if ((int)szBlock <= 0){
		return INVALID_PARAMETERS;
	} else {
		errCode = _take_free_space(ptr, szBlock);
	}

	return errCode;
}



long _take_free_space(VA* ptr, size_t szBlock)
{
	int errCode;
	
	if (ram.space == NULL) {
		return UNKNOW_ERROR;
	} else {
		if(ram.head == NULL) {
			ptr = ram.space;		//OUT

			errCode = _add_new_block(NULL, NULL, ptr, szBlock);

			return errCode;
		} else {
			long space, beginOfSpace, endOfSpace;

			segment* curSegm = ram.head;
			tableCell* curTabCell;
			
			while(curSegm -> next != NULL) {
				segment* nextSegm = curSegm -> next;
				tableCell* nextTabCell; 

				errCode = _find_table_cell_by_segment_number(curSegm -> segmentNumber, curTabCell);

				if (errCode != SUCCESSFUL_EXECUTION) {
					return errCode;
				}

				errCode = _find_table_cell_by_segment_number(nextSegm -> segmentNumber, nextTabCell);

				if(errCode != SUCCESSFUL_EXECUTION) {
					return errCode;
				}

				beginOfSpace = curTabCell -> segmentSize + curTabCell -> offset;
				endOfSpace = nextTabCell -> offset;
				space = endOfSpace - beginOfSpace;

				if (space >= (long) szBlock) {
					ptr = curTabCell -> physAddr + curTabCell -> segmentSize;		//OUT

					errCode = _add_new_block(curSegm, curTabCell, ptr, szBlock);

					return errCode;
				}

				curSegm = nextSegm;
			}

			beginOfSpace = curTabCell -> segmentSize + curTabCell -> offset;
			endOfSpace = ram.size;
			space = endOfSpace - beginOfSpace;

			if (space >= (long) szBlock) {
				segment* lastSegm = curSegm;
				tableCell* lastTabCell;
				
				errCode = _find_table_cell_by_segment_number(lastSegm -> segmentNumber, lastTabCell);

				if (errCode != SUCCESSFUL_EXECUTION) {
					return errCode;
				}

				ptr = lastTabCell -> physAddr + lastTabCell -> segmentSize;		//OUT

				errCode = _add_new_block(curSegm, curTabCell, ptr, szBlock);

				return errCode;
			} else {
				return OUT_OF_MEMORY;
			}
		}
	}
}



int _find_table_cell_by_segment_number(int segmNumber, tableCell* tc)
{
	tc = table.head;

	while (tc != NULL) {
		if (tc -> segmentNumber == segmNumber) {
			return SUCCESSFUL_EXECUTION;
		}

		tc = tc -> next;
	}

	return INVALID_PARAMETERS;
}



int _add_new_block(segment* prevSegm, tableCell* prevTC, VA* ptr, size_t szBlock)
{
	tableCell* tc = (tableCell*) malloc(sizeof(tableCell));
	tableCell* nextTC;
	segment* segm = (segment*) malloc(sizeof(segment));
	segment* nextSegm;

	if (tc == NULL || segm == NULL) {
		return UNKNOW_ERROR;
	}

	//работа с инициализацией (ВЫНЕСТИ В ОТДЕЛЬНУЮ)
	tc -> physAddr = ptr;
	tc -> modification = 0;
	tc -> presence = 1;
	tc -> segmentSize = szBlock;

	//работа с двусвязным списком (ВЫНЕСТИ В ОТДЕЛЬНУЮ)
	if (prevTC != NULL) {
		nextTC = prevTC -> next;

		if (nextTC != NULL) {
			nextTC -> prev = tc;
			tc -> next = nextTC;
		} else {
			tc -> next = NULL;

			table.tail = tc;
		}

		prevTC -> next = tc;
		tc -> prev = prevTC;
	} else {
		tc -> next = NULL;
		tc -> prev = NULL;

		table.head = tc;
		table.tail = tc;
	}

	//работа с инициализацией (ВЫНЕСТИ В ОТДЕЛЬНУЮ)
	segm -> segmentNumber = tc -> segmentNumber = maxSegmentNumber++;

	//работа с двусвязным списком (ВЫНЕСТИ В ОТДЕЛЬНУЮ)
	if (prevSegm != NULL) {											//проверяем, первый ли это сегмент или нет
		nextSegm = prevSegm -> next;

		if (nextSegm != NULL) {
			nextSegm -> prev = segm;
			segm -> next = nextSegm;
		} else {
			segm -> next = NULL;

			ram.tail = segm;
		}

		prevSegm -> next = segm;
		segm -> prev = prevSegm;

		tc -> offset = prevTC -> segmentSize + prevTC -> offset;	//устанавливаем смещение текущего сегмента, как сумму размера и смещения предыдущего
	} else {
		segm -> next = NULL;
		segm -> prev = NULL;
		
		ram.head = segm;
		ram.tail = segm;

		tc -> offset = 0;											//если да, то устанавливаем смещение в 0
	}

	return SUCCESSFUL_EXECUTION;
}



int _free(VA ptr)
{
	tableCell* tc;
	segment* segm;
	int errCode;

	errCode = _find_table_cell_by_ptr(tc, ptr);
	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	errCode = _free_table_cell(tc);
	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	errCode = _find_segment_by_segment_number(segm, tc -> segmentNumber);
	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	errCode = _free_segment(segm);
	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}
}



int _find_table_cell_by_ptr(tableCell* tc, VA ptr) 
{
	tc = table.head;

	while (tc != NULL) {
		if (tc -> physAddr == &ptr) {
			return SUCCESSFUL_EXECUTION;
		} else {
			tc = tc -> next;
		}
	}

	return INVALID_PARAMETERS;
}



int _find_segment_by_segment_number(segment* segm, int segmNumber)
{
	segm = ram.head;

	while (segm != NULL) {
		if (segm -> segmentNumber == segmNumber) {
			return SUCCESSFUL_EXECUTION;
		}

		segm = segm -> next;
	}

	return INVALID_PARAMETERS;
}



int _free_table_cell(tableCell* tc)
{
	tableCell* prevTC = tc -> prev;
	tableCell* nextTC = tc -> next;

	if (tc == NULL) {
		return UNKNOW_ERROR;
	}
	
	if (prevTC != NULL && nextTC != NULL) {
		prevTC -> next = nextTC;
		nextTC -> prev = prevTC;
	} else if (prevTC == NULL && nextTC == NULL) {
		table.head = NULL;
		table.tail = NULL;
	}

	if (prevTC == NULL) {
		table.head = nextTC;
	} 
	
	if (nextTC == NULL) {
		table.tail = prevTC;
	}	

	tc -> prev = NULL;
	tc -> next = NULL;
	tc -> physAddr = NULL;

	free(tc);

	return SUCCESSFUL_EXECUTION;
}



int _free_segment(segment* segm)
{
	segment* prevSegm = segm -> prev;
	segment* nextSegm = segm -> next;

	if (segm == NULL) {
		return UNKNOW_ERROR;
	}
	
	if (prevSegm != NULL && nextSegm != NULL) {
		prevSegm -> next = nextSegm;
		nextSegm -> prev = prevSegm;
	} else if (prevSegm == NULL && nextSegm == NULL) {
		ram.head = NULL;
		ram.tail = NULL;
	}

	if (prevSegm == NULL) {
		ram.head = nextSegm;
	} 
	
	if (nextSegm == NULL) {
		ram.tail = prevSegm;
	}	

	segm -> prev = NULL;
	segm -> next = NULL;

	free(segm);

	return SUCCESSFUL_EXECUTION;
}