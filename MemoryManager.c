#include "stdlib.h"
#include "string.h"
#include "MemoryManager.h"

int _init (int n, int szPage)
{
	int errCode, ramSize, hardSize, cashRecNum;

	if (n <= 0 || szPage <= 0) {
		return INVALID_PARAMETERS;
	}

	ramSize = n * szPage, hardSize = 65536, cashRecNum = 5;

	errCode = _init_virtual_address_space(ramSize, hardSize);
	if(errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	/*errCode = _init_hard_drive();
	if(errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}*/

	errCode = _init_cash(cashRecNum);

	return errCode;
}



int _init_virtual_address_space(long ramSize, long hardSize)
{
	virtualAddressSpace* vas = &table.vas;

	if (vas -> space != NULL) {
		int errCode = _destroy_virtual_address_space();
		if (errCode != SUCCESSFUL_EXECUTION) {
			return errCode;
		}
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



int _destroy_virtual_address_space()
{
	virtualAddressSpace* vas = &table.vas;
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



int _init_cash(int cashRecNum)
{
	int num;
	int errCode;
	
	if (csh.head != NULL) {
		errCode = _destroy_cash();

		return errCode;
	}

	for (num = 0; num < cashRecNum; num++) {
		errCode = _add_cash_record();
		if (errCode != SUCCESSFUL_EXECUTION) {
			return errCode;
		}
	}

	return SUCCESSFUL_EXECUTION;
}



int _add_cash_record() {
	cashRecord* rec = (cashRecord*) malloc(sizeof(cashRecord));

	int errCode;

	if (rec == NULL) {
		return UNKNOW_ERROR;
	}

	rec -> modification = 0;
	rec -> reality = 0;

	if (csh.head == NULL && csh.tail == NULL) {
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

	return SUCCESSFUL_EXECUTION;
}



int _destroy_cash()
{
	cashRecord* rec = csh.head;
	int errCode;

	while (rec != NULL) {
		cashRecord* nextRec = rec -> next;

		rec -> modification = 0;
		rec -> reality = 0;

		rec = nextRec;
	}

	return SUCCESSFUL_EXECUTION;
}



int _malloc (VA* ptr, size_t szBlock)
{
	virtualAddressSpace* vas = &table.vas;
	int errCode;

	if ((int)szBlock <= 0){
		return INVALID_PARAMETERS;
	} else if (vas -> space == NULL) {
		return UNKNOW_ERROR;
	} else if (vas -> ramSize < (long) szBlock) {
		return OUT_OF_MEMORY;
	} else {
		errCode = _take_free_space(ptr, szBlock);
	}

	return errCode;
}



int _take_free_space(VA* ptr, size_t szBlock)
{
	int errCode;
	virtualAddressSpace* vas = &table.vas;
	long space, beginOfSpace, endOfSpace;

	if (vas -> head == NULL) {
		(*ptr) = vas -> space;		//OUT

		errCode = _add_new_block(NULL, NULL, ptr, szBlock);

		return errCode;
	} else {
		segment* curSegm = vas -> head;

		beginOfSpace = 0;
		endOfSpace = curSegm -> offset;
		space = endOfSpace - beginOfSpace;

		if (space >= (long) szBlock) {
			(*ptr) = vas -> space;		//OUT

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
		endOfSpace = vas -> ramSize + table.vas.hardSize;
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
	if(errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	errCode = _add_hard_segment(szBlock);

	maxSegmentNumber++;

	return errCode;
}



int _add_table_cell(tableCell* prevTC, tableCell* nextTC, size_t szBlock) {
	virtualAddressSpace* vas = &table.vas;
	tableCell* tc = (tableCell*) malloc(sizeof(tableCell));

	if (tc == NULL) {
		return UNKNOW_ERROR;
	}

	tc -> modification = 0;
	tc -> segmentSize = szBlock;
	tc -> segmentNumber = maxSegmentNumber;

	if (vas -> ramFree < (long) szBlock) {
		tc -> presence = 0;
		vas -> hardFree -= (long) szBlock;

		tc -> physAddr = NULL;
	} else {
		tc -> presence = 1;
		vas -> ramFree -= (long) szBlock;

		tc -> physAddr = (VA) malloc((long)szBlock);
		if (tc -> physAddr == NULL) {
			return UNKNOW_ERROR;
		}
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
	virtualAddressSpace* vas = &table.vas;
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

		vas -> head = segm;
		vas -> tail = segm;

		segm -> offset = 0;
	} else if (prevSegm == NULL) {
		segm -> prev = NULL;
		segm -> next = nextSegm;

		nextSegm -> prev = segm;

		vas -> head = segm;

		segm -> offset = 0;
	} else if (nextSegm == NULL) {
		segm -> prev = prevSegm;
		segm -> next = NULL;

		prevSegm -> next = segm;

		vas -> tail = segm;

		segm -> offset = prevSegm -> segmentSize + prevSegm -> offset;
	}

	return SUCCESSFUL_EXECUTION;
}



int _add_hard_segment(size_t szBlock) {
	hardSegment* hardSegm = (hardSegment*) malloc(sizeof(hardSegment));

	if (hardSegm == NULL) {
		return UNKNOW_ERROR;
	}

	hardSegm -> data = (VA) malloc((long)szBlock);

	if (hardSegm -> data == NULL) {
		return UNKNOW_ERROR;
	}

	hardSegm -> segmentNumber = maxSegmentNumber;
	hardSegm -> segmentSize = (long) szBlock;

	if (drive.head == NULL && drive.tail == NULL) {
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

	return SUCCESSFUL_EXECUTION;
}



int _free(VA ptr)
{
	tableCell* tc = NULL;
	segment* segm = NULL;
	hardSegment* hardSegm = NULL;
	int errCode;

	errCode = _find_segment_by_ptr(&segm, ptr);
	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	errCode = _find_table_cell_by_segment_number(&tc, segm -> segmentNumber);
	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	errCode = _find_hard_segment_by_segment_number(&hardSegm, segm -> segmentNumber);
	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	errCode = _free_table_cell(&tc);
	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	errCode = _free_segment(&segm);
	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	errCode = _free_hard_segment(&hardSegm);
	
	return errCode;
}



int _free_table_cell(tableCell** tc)
{
	virtualAddressSpace* vas = &table.vas;
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
		vas -> hardFree += (*tc) -> segmentSize;
	} else {
		free((*tc) -> physAddr);

		vas -> ramFree += (*tc) -> segmentSize;
	}

	free((*tc));

	(*tc) = NULL;

	return SUCCESSFUL_EXECUTION;
}



int _free_segment(segment** segm)
{
	virtualAddressSpace* vas = &table.vas;
	segment* prevSegm = (*segm) -> prev;
	segment* nextSegm = (*segm) -> next;
	
	if (prevSegm != NULL && nextSegm != NULL) {
		prevSegm -> next = nextSegm;
		nextSegm -> prev = prevSegm;
	} else if (prevSegm == NULL && nextSegm == NULL) {
		vas -> head = NULL;
		vas -> tail = NULL;
	} else if (prevSegm == NULL) {
		nextSegm -> prev = NULL;
		vas -> head = nextSegm;
	} else if (nextSegm == NULL) {
		prevSegm -> next = NULL;
		vas -> tail = prevSegm;
	}

	free((*segm));

	(*segm) = NULL;

	return SUCCESSFUL_EXECUTION;
}



int _free_hard_segment(hardSegment** hardSegm)
{
	hardSegment* prevHardSegm = (*hardSegm) -> prev;
	hardSegment* nextHardSegm = (*hardSegm) -> next;

	if (prevHardSegm != NULL && nextHardSegm != NULL) {
		prevHardSegm -> next = nextHardSegm;
		nextHardSegm -> prev = prevHardSegm;
	} else if (prevHardSegm == NULL && nextHardSegm == NULL) {
		drive.head = NULL;
		drive.tail = NULL;
	} else if (prevHardSegm == NULL) {
		nextHardSegm -> prev = NULL;
		drive.head = nextHardSegm;
	} else if (nextHardSegm == NULL) {
		prevHardSegm -> next = NULL;
		drive.tail = prevHardSegm;
	}

	free((*hardSegm) -> data);

	free((*hardSegm));

	(*hardSegm) = NULL;

	return SUCCESSFUL_EXECUTION;
}



int _write (VA ptr, void* pBuffer, size_t szBuffer) 
{
	virtualAddressSpace* vas = &table.vas;
	segment* segm;
	tableCell* tc;
	long offset = ptr - vas -> space;
	long vasSize = vas -> ramSize + vas -> hardSize;
	long offsetInSegm;
	int errCode;

	if (offset < 0 || offset >= vasSize || ptr == NULL) {
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

	offsetInSegm = ptr - segm -> virtAddr;

	if ((long) szBuffer > segm -> segmentSize - offsetInSegm) {
		return OUT_OF_BLOCK_RANGE;
	}

	if (tc -> presence == 0) {
		errCode = _free_space_to_load_segment(&segm); //освобождение места в ОП под сегмент
		if (errCode != SUCCESSFUL_EXECUTION) {
			return errCode;
		}

		errCode = _load_segment_to_ram(&tc);
		if (errCode != SUCCESSFUL_EXECUTION) {
			return errCode;
		}
	}

	memcpy(tc -> physAddr + offsetInSegm, (PA) pBuffer, szBuffer);

	tc -> modification = 1;

	return SUCCESSFUL_EXECUTION;
}



int _read (VA ptr, void* pBuffer, size_t szBuffer)
{
	virtualAddressSpace* vas = &table.vas;
	segment* segm;
	tableCell* tc;
	long offset = ptr - vas -> space;
	long vasSize = vas -> ramSize + vas -> hardSize;
	long offsetInSegm;
	int errCode;

	if (offset < 0 || offset >= vasSize || ptr == NULL) {
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

	offsetInSegm = ptr - segm -> virtAddr;

	if ((long) szBuffer > segm -> segmentSize - offsetInSegm) {
		return OUT_OF_BLOCK_RANGE;
	}

	if (tc -> presence == 0) {
		errCode = _free_space_to_load_segment(&segm); //освобождение места в ОП под сегмент
		if (errCode != SUCCESSFUL_EXECUTION) {
			return errCode;
		}

		errCode = _load_segment_to_ram(&tc);
		if (errCode != SUCCESSFUL_EXECUTION) {
			return errCode;
		}
	}

	memcpy((PA) pBuffer, tc -> physAddr + offsetInSegm, szBuffer);

	return SUCCESSFUL_EXECUTION;
}



int _free_space_to_load_segment(segment** segm) 
{
	virtualAddressSpace* vas = &table.vas;
	tableCell* tc = NULL;

	int errCode = _find_table_cell_by_segment_number(&tc, (*segm) -> segmentNumber);
	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	if (vas -> ramFree >= (*segm) -> segmentSize) {
		return SUCCESSFUL_EXECUTION;
	} else {
		tableCell* curTC = table.head;

		while(curTC != NULL) {
			if (curTC -> presence == 1 && curTC -> segmentSize + vas -> ramFree >= (*segm) -> segmentSize) {
				errCode = _load_segment_to_hard_drive(&curTC);

				return errCode;
			}

			curTC = curTC -> next;
		}

		curTC = table.head;

		while(curTC != NULL && vas -> ramFree < (*segm) -> segmentSize) {
			if (curTC -> presence == 1) {
				errCode = _load_segment_to_hard_drive(&curTC);
				if (errCode != SUCCESSFUL_EXECUTION) {
					return errCode;
				}
			}

			curTC = curTC -> next;
		}
		
		return SUCCESSFUL_EXECUTION;
	}
}



int _load_segment_to_ram(tableCell** tc)	//моделируем загрузку сегмента с Ж/Д в ОП
{		
	virtualAddressSpace* vas = &table.vas;
	hardSegment* hardSegm = NULL;
	PA tempBuff = (PA) malloc((*tc) -> segmentSize); 
	int errCode;

	errCode = _find_hard_segment_by_segment_number(&hardSegm, (*tc) -> segmentNumber);
	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	memcpy(tempBuff, hardSegm -> data, (*tc) -> segmentSize);

	(*tc) -> physAddr = (PA) malloc((*tc) -> segmentSize);
	if ((*tc) -> physAddr == NULL) {
		return UNKNOW_ERROR;
	}

	memcpy((*tc) -> physAddr, tempBuff, (*tc) -> segmentSize);
	free(tempBuff);

	vas -> ramFree -= (*tc) -> segmentSize;

	(*tc) -> presence = 1;

	return SUCCESSFUL_EXECUTION;
}



int _load_segment_to_hard_drive(tableCell** tc)	//моделируем загрузку сегмента из ОП на Ж/Д
{		
	virtualAddressSpace* vas = &table.vas;
	hardSegment* hardSegm = NULL;
	int errCode;

	if ((*tc) -> modification == 1) {
		PA tempBuff = (PA) malloc((*tc) -> segmentSize); 

		memcpy(tempBuff, (*tc) -> physAddr, (*tc) -> segmentSize);

		errCode = _find_hard_segment_by_segment_number(&hardSegm, (*tc) -> segmentNumber);
		if (errCode != SUCCESSFUL_EXECUTION) {
			return errCode;
		}

		memcpy(hardSegm -> data, tempBuff, (*tc) -> segmentSize);

		free(tempBuff);
	}

	free((*tc) -> physAddr);

	vas -> ramFree += (*tc) -> segmentSize;

	(*tc) -> presence = 0;
	(*tc) -> modification = 0;

	return SUCCESSFUL_EXECUTION;
}



int _find_segment_by_ptr(segment** segm, VA ptr)
{
	(*segm) = table.vas.head;

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



int _find_hard_segment_by_segment_number(hardSegment** hardSegm, int segmNumber)
{
	(*hardSegm) = drive.head;

	while ((*hardSegm) != NULL) {
		if ((*hardSegm) -> segmentNumber == segmNumber) {
			return SUCCESSFUL_EXECUTION;
		}

		(*hardSegm) = (*hardSegm) -> next;
	}

	return INVALID_PARAMETERS;
}