#include "stdlib.h"
#include "string.h"
#include "MemoryManager.h"

int _init (int n, int szPage)
{
	int errCode, ramSize, hardSize;

	if (n <= 0 || szPage <= 0) {
		return INVALID_PARAMETERS;
	}

	ramSize = n * szPage, hardSize = 65536;

	errCode = _init_virtual_address_space(&vas, ramSize, hardSize);

	return errCode;
}



int _init_virtual_address_space(virtualAddressSpace* vas, long ramSize, long hardSize)
{
	if (vas -> space != NULL) {
		_destroy(vas);
	}

	vas -> space = (VA) malloc(ramSize + hardSize);

	if (vas -> space == NULL){
		return UNKNOW_ERROR;
	} else {
		vas -> head = NULL;
		vas -> tail = NULL;
		vas -> hardSize = hardSize;
		vas -> ramSize = ramSize;
		vas -> hardFree = hardSize;
		vas -> ramFree = ramSize;

		return SUCCESSFUL_EXECUTION;
	}
}



int _destroy(virtualAddressSpace* vas)
{
	segment* segm = vas -> head;
	int errCode;

	while (segm != NULL) {
		segment* nextSegm = segm -> next;

		errCode = _free(segm -> virtAddr);

		if (errCode != SUCCESSFUL_EXECUTION) {
			return errCode;
		}

		segm = nextSegm;
	} 

	if (vas -> space != NULL) {
		free(vas -> space);
		vas -> space = NULL;
	}

	maxSegmentNumber = 0;

	return SUCCESSFUL_EXECUTION;
}



int _malloc (VA* ptr, size_t szBlock)
{
	int errCode;

	if ((int)szBlock <= 0){
		return INVALID_PARAMETERS;
	} else if (vas.space == NULL) {
		return UNKNOW_ERROR;
	} else if (vas.ramSize < (long) szBlock) {
		return OUT_OF_MEMORY;
	} else {
		errCode = _take_free_space(ptr, szBlock);
	}

	return errCode;
}



int _take_free_space(VA* ptr, size_t szBlock)
{
	int errCode;
	long space, beginOfSpace, endOfSpace;

	if (vas.head == NULL) {
		(*ptr) = vas.space;		//OUT

		errCode = _add_new_block(NULL, NULL, ptr, szBlock);

		return errCode;
	} else {
		segment* curSegm = vas.head;

		beginOfSpace = 0;
		endOfSpace = curSegm -> offset;
		space = endOfSpace - beginOfSpace;

		if (space >= (long) szBlock) {
			(*ptr) = vas.space;		//OUT

			errCode = _add_new_block(NULL, curSegm, ptr, szBlock);

			return errCode;
		}
			
		while(curSegm -> next != NULL) {
			segment* nextSegm = curSegm -> next;

			beginOfSpace = curSegm -> segmentSize + curSegm -> offset;
			endOfSpace = nextSegm -> offset;
			space = endOfSpace - beginOfSpace;

			if (space >= (long) szBlock) {
				(*ptr) = curSegm -> virtAddr + curSegm -> segmentSize;		//OUT

				errCode = _add_new_block(curSegm, nextSegm, ptr, szBlock);

				return errCode;
			}

			curSegm = nextSegm;
		}

		beginOfSpace = curSegm -> segmentSize + curSegm -> offset;
		endOfSpace = vas.ramSize + vas.hardSize;
		space = endOfSpace - beginOfSpace;

		if (space >= (long) szBlock) {
			(*ptr) = curSegm -> virtAddr + curSegm -> segmentSize;		//OUT

			errCode = _add_new_block(curSegm, NULL, ptr, szBlock);

			return errCode;
		}
	}

	return OUT_OF_MEMORY;
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

	errCode = _add_table_cell(prevTC, nextTC, szBlock);

	if(errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	errCode = _add_segment(prevSegm, nextSegm, ptr, szBlock);

	maxSegmentNumber++;

	return errCode;
}



int _add_table_cell(tableCell* prevTC, tableCell* nextTC, size_t szBlock) {
	tableCell* tc = (tableCell*) malloc(sizeof(tableCell));

	if (tc == NULL) {
		return UNKNOW_ERROR;
	}

	tc -> physAddr = (VA) malloc((long)szBlock);

	if (tc -> physAddr == NULL) {
		return UNKNOW_ERROR;
	}

	tc -> modification = 0;
	tc -> segmentSize = szBlock;
	tc -> segmentNumber = maxSegmentNumber;

	if (vas.ramFree < (long) szBlock) {
		tc -> presence = 0;
		vas.hardFree -= (long) szBlock;
	} else {
		tc -> presence = 1;
		vas.ramFree -= (long) szBlock;
	}

	if (prevTC != NULL && nextTC != NULL) {
		tc -> prev = prevTC;
		tc -> next = nextTC;

		nextTC -> prev = tc;
		prevTC -> next = tc;
	} else if (prevTC == NULL && nextTC == NULL) {
		tc -> next = NULL;
		tc -> prev = NULL;

		table.head = tc;
		table.tail = tc;
	} else if (prevTC == NULL) {
		tc -> prev = NULL;
		tc -> next = nextTC;

		nextTC -> prev = tc;

		table.head = tc;
	} else if (nextTC == NULL) {
		tc -> prev = prevTC;
		tc -> next = NULL;

		prevTC -> next = tc;

		table.tail = tc;
	}

	return SUCCESSFUL_EXECUTION;
}



int _add_segment(segment* prevSegm, segment* nextSegm, VA* ptr, size_t szBlock) {
	segment* segm = (segment*) malloc(sizeof(segment));

	if (segm == NULL) {
		return UNKNOW_ERROR;
	}

	segm -> virtAddr = (*ptr);
	segm -> segmentNumber = maxSegmentNumber;
	segm -> segmentSize = szBlock;

	if (prevSegm != NULL && nextSegm != NULL) {
		segm -> prev = prevSegm;
		segm -> next = nextSegm;

		prevSegm -> next = segm;
		nextSegm -> prev = segm;

		segm -> offset = prevSegm -> segmentSize + prevSegm -> offset;
	} else if (prevSegm == NULL && nextSegm == NULL) {
		segm -> next = NULL;
		segm -> prev = NULL;

		vas.head = segm;
		vas.tail = segm;

		segm -> offset = 0;
	} else if (prevSegm == NULL) {
		segm -> prev = NULL;
		segm -> next = nextSegm;

		nextSegm -> prev = segm;

		vas.head = segm;

		segm -> offset = 0;
	} else if (nextSegm == NULL) {
		segm -> prev = prevSegm;
		segm -> next = NULL;

		prevSegm -> next = segm;

		vas.tail = segm;

		segm -> offset = prevSegm -> segmentSize + prevSegm -> offset;
	}

	return SUCCESSFUL_EXECUTION;
}



int _free(VA ptr)
{
	tableCell* tc = NULL;
	segment* segm = NULL;
	int errCode;

	errCode = _find_segment_by_ptr(&segm, ptr);
	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	errCode = _find_table_cell_by_segment_number(&tc, segm -> segmentNumber);
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

	if ((*tc) -> presence == 0) {
		vas.hardFree -= (*tc) -> segmentSize;
	} else {
		vas.ramFree -= (*tc) -> segmentSize;
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
		vas.head = NULL;
		vas.tail = NULL;
	} else if (prevSegm == NULL) {
		nextSegm -> prev = NULL;
		vas.head = nextSegm;
	} else if (nextSegm == NULL) {
		prevSegm -> next = NULL;
		vas.tail = prevSegm;
	}

	free((*segm));

	(*segm) = NULL;

	return SUCCESSFUL_EXECUTION;
}



int _write (VA ptr, void* pBuffer, size_t szBuffer) 
{
	segment* segm;
	tableCell* tc;
	long offsetInSegm = ptr - vas.space, vasSize = vas.ramSize + vas.hardSize;
	int errCode;

	if (offsetInSegm < 0 || offsetInSegm >= vasSize || ptr == NULL) {
		return INVALID_PARAMETERS;
	}

	errCode = _find_segment_by_ptr(&segm, ptr);
	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	errCode = _find_table_cell_by_segment_number(&tc, segm -> segmentNumber);
	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	if ((long) szBuffer > segm -> segmentSize - offsetInSegm) {
		return OUT_OF_BLOCK_RANGE;
	}

	if (tc -> presence == 0) {
		//загрузка сегмента с Ж/Д в ОП
	} else {
		memcpy(tc -> physAddr + offsetInSegm, (PA) pBuffer, szBuffer);
	}

	return SUCCESSFUL_EXECUTION;
}



int _find_segment_by_ptr(segment** segm, VA ptr)
{
	(*segm) = vas.head;

	while ((*segm) != NULL) {
		long offsetInSegm = ptr - (*segm) -> virtAddr;

		if (offsetInSegm >= 0 && offsetInSegm < (*segm) -> segmentSize) {
			return SUCCESSFUL_EXECUTION;
		} else {
			(*segm) = (*segm) -> next;
		}
	}

	return INVALID_PARAMETERS;
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