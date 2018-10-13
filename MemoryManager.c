#include "stdlib.h"
#include "MemoryManager.h"

int _init (int n, int szPage)
{
	int errCode, ramSize, hardSize, cashSize;

	if (n <= 0 || szPage <= 0) {
		return INVALID_PARAMETERS;
	}

	ramSize = n * szPage, hardSize = 65536, cashSize = 2048;

	errCode = _init_physical_memory(ramSize, &ram);

	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	errCode = _init_physical_memory(hardSize, &hard);

	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	errCode = _init_physical_memory(cashSize, &cash);

	return errCode;
}



int _init_physical_memory(int memorySize, physicalMemory* mem)
{
	if (mem -> space != NULL) {
		_destroy(mem);
	}

	mem -> space = (VA) malloc(memorySize);

	if (mem -> space == NULL){
		return UNKNOW_ERROR;
	} else {
		mem -> head = NULL;
		mem -> tail = NULL;
		mem -> size = memorySize;

		return SUCCESSFUL_EXECUTION;
	}
}



int _destroy(physicalMemory* mem)
{
	tableCell* tc = table.head;
	int errCode;

	while (tc != NULL) {
		tableCell* nextTC = tc -> next;

		errCode = _free(tc -> physAddr);

		if (errCode != SUCCESSFUL_EXECUTION) {
			return errCode;
		}

		tc = nextTC;
	} 

	if (mem -> space != NULL) {
		free(mem -> space);
		mem -> space = NULL;
	}

	return SUCCESSFUL_EXECUTION;
}



int _malloc (VA* ptr, size_t szBlock)
{
	int errCode;

	if ((int)szBlock <= 0){
		return INVALID_PARAMETERS;
	} else if (ram.space == NULL) {
		return UNKNOW_ERROR;
	} else {
		errCode = _take_free_space(ptr, szBlock);
	}

	return errCode;
}



int _take_free_space(VA* ptr, size_t szBlock)
{
	int errCode;
	long space, beginOfSpace, endOfSpace;

	if(ram.head == NULL) {
		beginOfSpace = 0;
		endOfSpace = ram.size;
		space = endOfSpace - beginOfSpace;

		if (space >= (long) szBlock) {
			(*ptr) = ram.space;		//OUT

			errCode = _add_new_block(NULL, NULL, ptr, szBlock);

			return errCode;
		}
	} else {
		segment* curSegm = ram.head;
		tableCell* curTabCell = NULL;

		errCode = _find_table_cell_by_segment_number(&curTabCell, curSegm -> segmentNumber);

		if (errCode != SUCCESSFUL_EXECUTION) {
			return errCode;
		}

		beginOfSpace = 0;
		endOfSpace = curTabCell -> offset;
		space = endOfSpace - beginOfSpace;

		if (space >= (long) szBlock) {
			(*ptr) = ram.space;		//OUT

			errCode = _add_new_block(NULL, curSegm, ptr, szBlock);

			return errCode;
		}
			
		while(curSegm -> next != NULL) {
			segment* nextSegm = curSegm -> next;
			tableCell* nextTabCell; 

			errCode = _find_table_cell_by_segment_number(&nextTabCell, nextSegm -> segmentNumber);

			if(errCode != SUCCESSFUL_EXECUTION) {
				return errCode;
			}

			beginOfSpace = curTabCell -> segmentSize + curTabCell -> offset;
			endOfSpace = nextTabCell -> offset;
			space = endOfSpace - beginOfSpace;

			if (space >= (long) szBlock) {
				(*ptr) = curTabCell -> physAddr + curTabCell -> segmentSize;		//OUT

				errCode = _add_new_block(curSegm, nextSegm, ptr, szBlock);

				return errCode;
			}

			curSegm = nextSegm;
			errCode = _find_table_cell_by_segment_number(&curTabCell, curSegm -> segmentNumber);

			if(errCode != SUCCESSFUL_EXECUTION) {
				return errCode;
			}
		}

		beginOfSpace = curTabCell -> segmentSize + curTabCell -> offset;
		endOfSpace = ram.size;
		space = endOfSpace - beginOfSpace;

		if (space >= (long) szBlock) {
			(*ptr) = curTabCell -> physAddr + curTabCell -> segmentSize;		//OUT

			errCode = _add_new_block(curSegm, NULL, ptr, szBlock);

			return errCode;
		}
	}

	return OUT_OF_MEMORY;
}



int _find_table_cell_by_segment_number(tableCell** tc, int segmNumber)
{
	(*tc) = table.head;

	while ((*tc) != NULL) {
		if ((*tc) -> segmentNumber == segmNumber) {
			return SUCCESSFUL_EXECUTION;
		}

		(*tc) = (*tc) -> next;
	}

	return INVALID_PARAMETERS;
}



int _add_new_block(segment* prevSegm, segment* nextSegm, VA* ptr, size_t szBlock)
{
	tableCell* prevTC = NULL;
	tableCell* nextTC = NULL;
	int errCode;

	if (prevSegm != NULL) {
		errCode = _find_table_cell_by_segment_number(&prevTC, prevSegm -> segmentNumber);

		if(errCode != SUCCESSFUL_EXECUTION) {
			return errCode;
		}
	}

	if (nextSegm != NULL) {
		errCode = _find_table_cell_by_segment_number(&nextTC, nextSegm -> segmentNumber);

		if(errCode != SUCCESSFUL_EXECUTION) {
			return errCode;
		}
	}

	errCode = _add_table_cell(prevTC, nextTC, ptr, szBlock);

	if(errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	errCode = _add_segment(prevSegm, nextSegm);

	maxSegmentNumber++;

	return errCode;
}



int _add_table_cell(tableCell* prevTC, tableCell* nextTC, VA* ptr, size_t szBlock) {
	tableCell* tc = (tableCell*) malloc(sizeof(tableCell));

	if (tc == NULL) {
		return UNKNOW_ERROR;
	}

	tc -> physAddr = *ptr;
	tc -> modification = 0;
	tc -> presence = 1;
	tc -> segmentSize = szBlock;
	tc -> segmentNumber = maxSegmentNumber;

	if (prevTC != NULL && nextTC != NULL) {
		tc -> prev = prevTC;
		tc -> next = nextTC;

		nextTC -> prev = tc;
		prevTC -> next = tc;

		tc -> offset = prevTC -> segmentSize + prevTC -> offset;
	} else if (prevTC == NULL && nextTC == NULL) {
		tc -> next = NULL;
		tc -> prev = NULL;

		table.head = tc;
		table.tail = tc;

		tc -> offset = 0;
	} else if (prevTC == NULL) {
		tc -> prev = NULL;
		tc -> next = nextTC;

		nextTC -> prev = tc;

		table.head = tc;

		tc -> offset = 0;
	} else if (nextTC == NULL) {
		tc -> prev = prevTC;
		tc -> next = NULL;

		prevTC -> next = tc;

		table.tail = tc;

		tc -> offset = prevTC -> segmentSize + prevTC -> offset;
	}

	return SUCCESSFUL_EXECUTION;
}



int _add_segment(segment* prevSegm, segment* nextSegm) {
	segment* segm = (segment*) malloc(sizeof(segment));

	if (segm == NULL) {
		return UNKNOW_ERROR;
	}

	segm -> segmentNumber = maxSegmentNumber;

	if (prevSegm != NULL && nextSegm != NULL) {
		segm -> prev = prevSegm;
		segm -> next = nextSegm;

		prevSegm -> next = segm;
		nextSegm -> prev = segm;
	} else if (prevSegm == NULL && nextSegm == NULL) {
		segm -> next = NULL;
		segm -> prev = NULL;

		ram.head = segm;
		ram.tail = segm;
	} else if (prevSegm == NULL) {
		segm -> prev = NULL;
		segm -> next = nextSegm;

		nextSegm -> prev = segm;

		ram.head = segm;
	} else if (nextSegm == NULL) {
		segm -> prev = prevSegm;
		segm -> next = NULL;

		prevSegm -> next = segm;

		ram.tail = segm;
	}

	return SUCCESSFUL_EXECUTION;
}



int _free(VA ptr)
{
	tableCell* tc = NULL;
	segment* segm = NULL;
	int errCode;

	errCode = _find_table_cell_by_ptr(&tc, ptr);
	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	errCode = _find_segment_by_segment_number(&segm, tc -> segmentNumber);
	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	errCode = _free_table_cell(&tc);
	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	errCode = _free_segment(&segm);
	
	return errCode;
}



int _find_table_cell_by_ptr(tableCell** tc, VA ptr) 
{
	(*tc) = table.head;

	while ((*tc) != NULL) {
		if ((*tc) -> physAddr == ptr) {
			return SUCCESSFUL_EXECUTION;
		} else {
			(*tc) = (*tc) -> next;
		}
	}

	return INVALID_PARAMETERS;
}



int _find_segment_by_segment_number(segment** segm, int segmNumber)
{
	(*segm) = ram.head;

	while ((*segm) != NULL) {
		if ((*segm) -> segmentNumber == segmNumber) {
			return SUCCESSFUL_EXECUTION;
		}

		(*segm) = (*segm) -> next;
	}

	return INVALID_PARAMETERS;
}



int _free_table_cell(tableCell** tc)
{
	tableCell* prevTC = (*tc) -> prev;
	tableCell* nextTC = (*tc) -> next;
	
	if (prevTC != NULL && nextTC != NULL) {
		prevTC -> next = nextTC;
		nextTC -> prev = prevTC;
	} else if (prevTC == NULL && nextTC == NULL) {
		table.head = NULL;
		table.tail = NULL;
	} else if (prevTC == NULL) {
		nextTC -> prev = NULL;
		table.head = nextTC;
	} else if (nextTC == NULL) {
		prevTC -> next = NULL;
		table.tail = prevTC;
	}	

	free((*tc));

	(*tc) = NULL;

	return SUCCESSFUL_EXECUTION;
}



int _free_segment(segment** segm)
{
	segment* prevSegm = (*segm) -> prev;
	segment* nextSegm = (*segm) -> next;
	
	if (prevSegm != NULL && nextSegm != NULL) {
		prevSegm -> next = nextSegm;
		nextSegm -> prev = prevSegm;
	} else if (prevSegm == NULL && nextSegm == NULL) {
		ram.head = NULL;
		ram.tail = NULL;
	} else if (prevSegm == NULL) {
		nextSegm -> prev = NULL;
		ram.head = nextSegm;
	} else if (nextSegm == NULL) {
		prevSegm -> next = NULL;
		ram.tail = prevSegm;
	}	

	free((*segm));

	(*segm) = NULL;

	return SUCCESSFUL_EXECUTION;
}