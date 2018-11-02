#include "stdlib.h"
#include "string.h"
#include "MemoryManager.h"

int _init (int n, int szPage)
{
	int size;

	if (n <= 0) {
		return INVALID_PARAMETERS;
	}

	if (szPage <= 0) {
		return INVALID_PARAMETERS;
	}

	size = n * szPage;

	if (size > hardSize) {
		return INVALID_PARAMETERS;
	}

	_init_virtual_address_space(size);

	_init_cash();

	return SUCCESSFUL_EXECUTION;
}



int _init_virtual_address_space(long size)
{
	virtualAddressSpace* vas = &table.vas;

	if (vas -> space != NULL) {
		_destroy_virtual_address_space();
	}

	vas -> space = (VA) malloc(size);

	if (vas -> space == NULL){
		return UNKNOW_ERROR;
	} else {
		vas -> head = NULL;
		vas -> tail = NULL;
		vas -> size = size;
		vas -> hardFree = hardSize;
		vas -> ramFree = ramSize;

		return SUCCESSFUL_EXECUTION;
	}
}



int _destroy_virtual_address_space()
{
	virtualAddressSpace* vas = &table.vas;
	segment* segm = vas -> head;

	while (segm != NULL) {
		segment* nextSegm = segm -> next;

		_free(segm -> virtAddr);

		segm = nextSegm;
	} 

	if (vas -> space != NULL) {
		free(vas -> space);
		vas -> space = NULL;
	}

	curSegmentNumber = 0;

	return SUCCESSFUL_EXECUTION;
}



int _init_cash()
{
	int recNum = 0;
	
	if (csh.head != NULL) {
		_destroy_cash();
	}

	for (recNum; recNum < maxRecordNumber; recNum++) {
		_add_cash_record();
	}

	return SUCCESSFUL_EXECUTION;
}



int _add_cash_record() {
	cashRecord* rec = (cashRecord*) malloc(sizeof(cashRecord));
	if (rec == NULL) {
		return UNKNOW_ERROR;
	}

	rec -> data = (PA) malloc(maxRecordSize);
	if (rec -> data == NULL) {
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

	while (rec != NULL) {
		cashRecord* nextRec = rec -> next;

		rec -> modification = 0;
		rec -> reality = 0;

		free(rec -> data);
		free(rec);

		rec = nextRec;
	}

	csh.head = NULL;
	csh.tail = NULL;

	return SUCCESSFUL_EXECUTION;
}



int _malloc (VA* ptr, size_t szBlock)
{
	virtualAddressSpace* vas = &table.vas;

	if ((long) szBlock <= 0){
		return INVALID_PARAMETERS;
	} else if (vas -> space == NULL) {
		return UNKNOW_ERROR;
	} else if (ramSize < (long) szBlock) {
		return OUT_OF_MEMORY;
	} else {
		return _take_free_space(ptr, szBlock);
	}
}



int _take_free_space(VA* ptr, size_t szBlock)
{
	int errCode;
	virtualAddressSpace* vas = &table.vas;
	long space, beginOfSpace, endOfSpace;

	if (vas -> head == NULL) {
		beginOfSpace = 0;
		endOfSpace = vas -> size;
		space = endOfSpace - beginOfSpace;

		if (space >= (long) szBlock) {
			(*ptr) = vas -> space;		//OUT

			_add_new_block(NULL, NULL, ptr, szBlock);

			return SUCCESSFUL_EXECUTION;
		}
	} else {
		segment* curSegm = vas -> head;

		beginOfSpace = 0;
		endOfSpace = curSegm -> offset;
		space = endOfSpace - beginOfSpace;

		if (space >= (long) szBlock) {
			(*ptr) = vas -> space;		//OUT

			_add_new_block(NULL, curSegm, ptr, szBlock);

			return SUCCESSFUL_EXECUTION;
		}
			
		while(curSegm -> next != NULL) {
			segment* nextSegm = curSegm -> next;

			beginOfSpace = curSegm -> segmentSize + curSegm -> offset;
			endOfSpace = nextSegm -> offset;
			space = endOfSpace - beginOfSpace;

			if (space >= (long) szBlock) {
				(*ptr) = curSegm -> virtAddr + curSegm -> segmentSize;		//OUT

				_add_new_block(curSegm, nextSegm, ptr, szBlock);

				return SUCCESSFUL_EXECUTION;
			}

			curSegm = nextSegm;
		}

		beginOfSpace = curSegm -> segmentSize + curSegm -> offset;
		endOfSpace = vas -> size;
		space = endOfSpace - beginOfSpace;

		if (space >= (long) szBlock) {
			(*ptr) = curSegm -> virtAddr + curSegm -> segmentSize;		//OUT

			_add_new_block(curSegm, NULL, ptr, szBlock);

			return SUCCESSFUL_EXECUTION;
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

	curSegmentNumber++;

	return errCode;
}



int _add_table_cell(tableCell* prevTC, tableCell* nextTC, size_t szBlock) 
{
	virtualAddressSpace* vas = &table.vas;
	tableCell* tc = (tableCell*) malloc(sizeof(tableCell));

	if (tc == NULL) {
		return UNKNOW_ERROR;
	}

	tc -> modification = 0;
	tc -> segmentSize = szBlock;
	tc -> segmentNumber = curSegmentNumber;

	if (vas -> ramFree < (long) szBlock) {
		tc -> presence = 0;
		
		tc -> physAddr = NULL;
	} else {
		tc -> presence = 1;
		vas -> ramFree -= (long) szBlock;

		tc -> physAddr = (PA) malloc((long)szBlock);
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



int _add_segment(segment* prevSegm, segment* nextSegm, VA* ptr, size_t szBlock)
{
	virtualAddressSpace* vas = &table.vas;
	segment* segm = (segment*) malloc(sizeof(segment));

	if (segm == NULL) {
		return UNKNOW_ERROR;
	}

	segm -> virtAddr = (*ptr);
	segm -> segmentNumber = curSegmentNumber;
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



int _add_hard_segment(size_t szBlock)
{
	virtualAddressSpace* vas = &table.vas;
	hardSegment* hardSegm = (hardSegment*) malloc(sizeof(hardSegment));

	if (hardSegm == NULL) {
		return UNKNOW_ERROR;
	}

	hardSegm -> data = (PA) malloc((long)szBlock);

	if (hardSegm -> data == NULL) {
		return UNKNOW_ERROR;
	}

	if (vas -> hardFree < (long) szBlock) {
		return OUT_OF_MEMORY;
	} else {
		vas -> hardFree -= (long) szBlock;
	}

	hardSegm -> segmentNumber = curSegmentNumber;
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
	cashRecord* rec = NULL;

	if (ptr == NULL) {
		return INVALID_PARAMETERS;
	}

	_find_segment_by_ptr(&segm, ptr);

	_find_table_cell_by_segment_number(&tc, segm -> segmentNumber);

	_find_hard_segment_by_segment_number(&hardSegm, segm -> segmentNumber);

	_find_cash_record_by_physical_address(&rec, tc -> physAddr);

	_free_table_cell(&tc);

	_free_segment(&segm);

	_free_hard_segment(&hardSegm);

	if (rec != NULL) {
		_free_cash_record(&rec);
	}

	return SUCCESSFUL_EXECUTION;
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

	if ((*tc) -> presence == 1) {
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
	virtualAddressSpace* vas = &table.vas;
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

	vas -> hardFree += (*hardSegm) -> segmentSize;

	free((*hardSegm));

	(*hardSegm) = NULL;

	return SUCCESSFUL_EXECUTION;
}



int _free_cash_record(cashRecord** rec) 
{
	if ((*rec) == NULL) {
		return UNKNOW_ERROR;
	}

	(*rec) -> reality = 0;
	(*rec) -> modification = 0;

	return SUCCESSFUL_EXECUTION;
}



int _write (VA ptr, void* pBuffer, size_t szBuffer) 
{
	virtualAddressSpace* vas = &table.vas;
	segment* segm;
	tableCell* tc;
	cashRecord* rec = NULL;
	long offset;
	long offsetInSegm;

	if (ptr == NULL) {
		return INVALID_PARAMETERS;
	}

	offset = ptr - vas -> space;

	if (offset < 0) {
		return INVALID_PARAMETERS;
	}

	if (offset >= vas -> size) {
		return INVALID_PARAMETERS;
	}

	_find_segment_by_ptr(&segm, ptr);

	offsetInSegm = ptr - segm -> virtAddr;

	if ((long) szBuffer > segm -> segmentSize - offsetInSegm) {
		return OUT_OF_BLOCK_RANGE;
	}

	_find_table_cell_by_segment_number(&tc, segm -> segmentNumber);

	if (tc -> presence == 0) {
		_free_space_to_load_segment(&segm); //освобождение места в ОП под сегмент

		_load_segment_to_ram(&tc);
	}

	_find_cash_record_by_physical_address(&rec, tc -> physAddr);

	if (rec != NULL) {
		memcpy(rec -> data + offsetInSegm, (PA) pBuffer, szBuffer);

		rec -> modification = 1;
	} else {
		memcpy(tc -> physAddr + offsetInSegm, (PA) pBuffer, szBuffer);

		tc -> modification = 1;

		_find_cash_record_to_load_segment(&rec, tc);

		if (rec != NULL) {
			_load_segment_to_cash(&tc, &rec);
		}
	}

	return SUCCESSFUL_EXECUTION;
}



int _read (VA ptr, void* pBuffer, size_t szBuffer)
{
	virtualAddressSpace* vas = &table.vas;
	segment* segm;
	tableCell* tc;
	cashRecord* rec = NULL;
	long offset;
	long offsetInSegm;

	if (ptr == NULL) {
		return INVALID_PARAMETERS;
	}

	offset = ptr - vas -> space;

	if (offset < 0) {
		return INVALID_PARAMETERS;
	}

	if (offset >= vas -> size) {
		return INVALID_PARAMETERS;
	}

	_find_segment_by_ptr(&segm, ptr);

	offsetInSegm = ptr - segm -> virtAddr;

	if ((long) szBuffer > segm -> segmentSize - offsetInSegm) {
		return OUT_OF_BLOCK_RANGE;
	}

	_find_table_cell_by_segment_number(&tc, segm -> segmentNumber);

	if (tc -> presence == 0) {
		_free_space_to_load_segment(&segm); //освобождение места в ОП под сегмент

		_load_segment_to_ram(&tc);
	}

	_find_cash_record_by_physical_address(&rec, tc -> physAddr);

	if (rec != NULL) {
		memcpy((PA) pBuffer, rec -> data + offsetInSegm, szBuffer);
	} else {
		memcpy((PA) pBuffer, tc -> physAddr + offsetInSegm, szBuffer);

		_find_cash_record_to_load_segment(&rec, tc);

		if (rec != NULL) {
			_load_segment_to_cash(&tc, &rec);
		}
	}

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
	int errCode;

	errCode = _find_hard_segment_by_segment_number(&hardSegm, (*tc) -> segmentNumber);
	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	(*tc) -> physAddr = (PA) malloc((*tc) -> segmentSize);
	if ((*tc) -> physAddr == NULL) {
		return UNKNOW_ERROR;
	}

	memcpy((*tc) -> physAddr, hardSegm -> data, (*tc) -> segmentSize);

	vas -> ramFree -= (*tc) -> segmentSize;

	(*tc) -> presence = 1;

	return SUCCESSFUL_EXECUTION;
}



int _load_segment_to_hard_drive(tableCell** tc)	//моделируем загрузку сегмента из ОП на Ж/Д
{		
	virtualAddressSpace* vas = &table.vas;
	hardSegment* hardSegm = NULL;
	cashRecord* rec = NULL;
	int errCode;

	errCode = _find_cash_record_by_physical_address(&rec, (*tc) -> physAddr);
	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	errCode = _find_hard_segment_by_segment_number(&hardSegm, (*tc) -> segmentNumber);
	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	if (rec != NULL && rec -> modification == 1) {
		memcpy(hardSegm -> data, rec -> data, (*tc) -> segmentSize);

		rec -> modification = 0;
		rec -> reality = 0;
	} else if ((*tc) -> modification == 1) {
		memcpy(hardSegm -> data, (*tc) -> physAddr, (*tc) -> segmentSize);
	}

	free((*tc) -> physAddr);

	vas -> ramFree += (*tc) -> segmentSize;

	(*tc) -> modification = 0;
	(*tc) -> presence = 0;

	return SUCCESSFUL_EXECUTION;
}



int _find_cash_record_to_load_segment(cashRecord** rec, tableCell* tc) {
	int errCode;
	(*rec) = csh.head;

	if (tc -> segmentSize > maxRecordSize) {
		(*rec) = NULL;
		return SUCCESSFUL_EXECUTION;
	}

	while ((*rec) != NULL) {
		if ((*rec) -> reality == 0) {
			return SUCCESSFUL_EXECUTION;
		}

		(*rec) = (*rec) -> next;
	}

	errCode = _load_segment_from_cash(rec);

	return errCode;
}



int _load_segment_from_cash(cashRecord** rec)
{
	tableCell* tc = NULL;
	int errCode;

	errCode = _find_cash_record_by_current_number(rec);
	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	errCode = _find_table_cell_by_physical_address(&tc, (*rec) -> physAddr);
	if (errCode != SUCCESSFUL_EXECUTION) {
		return errCode;
	}

	if ((*rec) -> modification == 1) {
		memcpy(tc -> physAddr, (*rec) -> data, tc -> segmentSize);

		(*rec) -> reality = 0;
		(*rec) -> modification = 0;
	}

	if (curRecordNumber == 5) {
		curRecordNumber = 0;
	} else {
		curRecordNumber++;
	}

	return SUCCESSFUL_EXECUTION;
}



int _load_segment_to_cash(tableCell** tc, cashRecord** rec)
{
	if ((*tc) == NULL || (*rec) == NULL) {
		return INVALID_PARAMETERS;
	}

	memcpy((*rec) -> data, (*tc) -> physAddr, (*tc) -> segmentSize);

	(*rec) -> reality = 1;
	(*rec) -> physAddr = (*tc) -> physAddr;

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



int _find_cash_record_by_physical_address(cashRecord** rec, PA physAddr)
{
	(*rec) = csh.head;

	while ((*rec) != NULL) {
		if ((*rec) -> physAddr == physAddr) {
			return SUCCESSFUL_EXECUTION;
		}

		(*rec) = (*rec) -> next;
	}

	return SUCCESSFUL_EXECUTION;
}



int _find_cash_record_by_current_number(cashRecord** rec)
{
	int recordNumber = 0;
	
	(*rec) = csh.head;

	while (recordNumber < curRecordNumber) {
		(*rec) = (*rec) -> next;
		recordNumber++;
	}

	return SUCCESSFUL_EXECUTION;
}



int _find_table_cell_by_physical_address(tableCell** tc, PA physAddr)
{
	(*tc) = table.head;

	while ((*tc) != NULL) {
		if ((*tc) -> physAddr == physAddr) {
			return SUCCESSFUL_EXECUTION;
		}

		(*tc) = (*tc) -> next;
	}

	return INVALID_PARAMETERS;
}