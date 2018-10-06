// MemoryManager.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "stdlib.h"

#define SUCCESSFUL_EXECUTION 0
#define INVALID_PARAMETERS -1
#define OUT_OF_BLOCK_RANGE -2
#define OUT_OF_MEMORY -2
#define UNKNOW_ERROR 1

typedef char* VA;						// Тип описывающий адрес блока (в ОП)

typedef struct tableCell
{
	int segmentNumber;					//номер сегмента процесса
	VA* physAddr;						//указатель на расположение в памяти
	long segmentSize;					//размер сегмента
	long offset;
	unsigned int modification;			//бит модификации сегмента
	unsigned int presence;				//бит присутствия сегмента в ОП
	struct tableCell* next;				//указатель на следующий сегмент
} tableCell;

typedef struct segment					//сегмент
{
	int segmentNumber;
	struct segment* next;
} segment;

typedef struct physicalMemory			//ОП, Ж/Д, кэш
{
	VA* space;							//собственно, указатель на память
	long size;							//размер
	segment* head;
} physicalMemory;



physicalMemory ram;
physicalMemory hard;
physicalMemory cash;
tableCell* table;



static int maxSegmentNumber = 0;



/**
 	@func	_malloc	
 	@brief	Выделяет блок памяти определенного размера
	
	@param	[out] ptr		адресс блока
	@param	[in]  szBlock	размер блока
	
	@return	код ошибки
	@retval	0	успешное выполнение
	@retval	-1	неверные параметры
	@retval	-2	нехватка памяти
	@retval	1	неизвестная ошибка
 **/
int _malloc (VA* ptr, size_t szBlock);



/**
 	@func	_free
 	@brief	Удаление блока памяти
	
	@param	[in] ptr		адресс блока
	
	@return	код ошибки
	@retval	0	успешное выполнение
	@retval	-1	неверные параметры
	@retval	1	неизвестная ошибка
 **/
int _free (VA ptr);



/**
 	@func	_read
 	@brief	Чтение информации из блока памяти
	
	@param	[in] ptr		адресс блока
	@param	[in] pBuffer	адресс буфера куда копируется инфомация
	@param	[in] szBuffer	размер буфера
	
	@return	код ошибки
	@retval	0	успешное выполнение
	@retval	-1	неверные параметры
	@retval	-2	доступ за пределы блока
	@retval	1	неизвестная ошибка
 **/
int _read (VA ptr, void* pBuffer, size_t szBuffer);



/**
 	@func	_write
 	@brief	Запись информации в блок памяти
	
	@param	[in] ptr		адресс блока
	@param	[in] pBuffer	адресс буфера куда копируется инфомация
	@param	[in] szBuffer	размер буфера
	
	@return	код ошибки
	@retval	0	успешное выполнение
	@retval	-1	неверные параметры
	@retval	-2	доступ за пределы блока
	@retval	1	неизвестная ошибка
 **/
int _write (VA ptr, void* pBuffer, size_t szBuffer);



/**
 	@func	_init
 	@brief	Инициализация модели менеджера памяти
	
	@param	[in] n		количество страниц
	@param	[in] szPage	размер страницы

	В варианте 1 и 2 общий объем памяти расчитывается как n*szPage
	
	@return	код ошибки
	@retval	0	успешное выполнение
	@retval	-1	неверные параметры
	@retval	1	неизвестная ошибка
 **/
int _init (int n, int szPage);



int _init_physical_memory(int memoryNumber, physicalMemory mem);
long _take_free_space(VA* ptr, size_t szBlock);
int _find_table_cell(int segmentNumber, tableCell* tc);
int _add_new_block(segment* prevSegm, tableCell* prevTC, VA* ptr, size_t szBlock);
int _find_last_segment(segment* segm);



int _tmain(int argc, _TCHAR* argv[])
{
	if(argc > 2){
		int n = (int)argv[1];
		int szPage = (int)argv[2];

		_init(n, szPage);
	}
	
	return 0;
}



int _init (int n, int szPage)
{
	int errCode, ramNumber = n * szPage, hardNumber = 65536, cashNumber = 2048;		//errCode - код ошибки

	errCode = _init_physical_memory(ramNumber, ram);		//инициализируем ОП

	_init_physical_memory(hardNumber, hard);		//инициализируем память жд

	_init_physical_memory(cashNumber, cash);		//инициализируем кэш

	return errCode;
}


int _init_physical_memory(int memoryNumber, physicalMemory mem)
{
	int errCode, memorySize = memoryNumber * sizeof(VA);

	mem.space = (VA*) malloc(memorySize);

	if(mem.space == NULL){
		errCode = 1;

	}
	else{
		errCode = 0;

		mem.head = NULL;
		mem.size = 0;
	}

	return errCode;
}



int _malloc (VA* ptr, size_t szBlock)
{
	int errCode;

	if(szBlock <= 0){
		errCode = -1;
	}
	else{
		VA* ptr;		//OUT
		errCode = _take_free_space(ptr, szBlock);
	}

	return errCode;
}



long _take_free_space(VA* ptr, size_t szBlock)
{
	int errCode;
	
	if(ram.space == NULL) {
		return UNKNOW_ERROR;
	} else {
		if(ram.head == NULL) {
			ptr = ram.space;		//OUT

			errCode = _add_new_block(NULL, NULL, ptr, szBlock);

			if(errCode != SUCCESSFUL_EXECUTION) {
				return errCode;
			}

			return errCode;
		} else {
			long space, beginOfSpace, endOfSpace;

			segment* curSegm = ram.head;
			tableCell* curTabCell; 
			errCode = _find_table_cell(curSegm -> segmentNumber, curTabCell);

			if(errCode != SUCCESSFUL_EXECUTION) {
				return errCode;
			}

			while(curSegm -> next != NULL) {
				segment* nextSegm = curSegm -> next;
				tableCell* nextTabCell; 

				errCode = _find_table_cell(nextSegm -> segmentNumber, nextTabCell);

				if(errCode != SUCCESSFUL_EXECUTION) {
					return errCode;
				}

				beginOfSpace = curTabCell -> segmentSize + curTabCell -> offset;
				endOfSpace = nextTabCell -> offset;
				space = endOfSpace - beginOfSpace;

				if(space >= (long) szBlock) {
					errCode = _find_table_cell(curSegm -> segmentNumber, curTabCell);

					if(errCode != SUCCESSFUL_EXECUTION) {
						return errCode;
					}

					ptr = curTabCell -> physAddr;		//OUT

					errCode = _add_new_block(curSegm, curTabCell, ptr, szBlock);

					if(errCode != SUCCESSFUL_EXECUTION) {
						return errCode;
					}

					return SUCCESSFUL_EXECUTION;
				}

				curSegm = nextSegm;
			}

			beginOfSpace = curTabCell -> segmentSize + curTabCell -> offset;
			endOfSpace = ram.size;
			space = endOfSpace - beginOfSpace;

			if(space >= (long) szBlock) {
				segment* lastSegm = curSegm;
				tableCell* lastTabCell;

				errCode = _find_table_cell(lastSegm -> segmentNumber, lastTabCell);

				if(errCode != SUCCESSFUL_EXECUTION) {
					return errCode;
				}

				ptr = lastTabCell -> physAddr + lastTabCell -> segmentSize;		//OUT

				errCode = _add_new_block(curSegm, curTabCell, ptr, szBlock);

				if(errCode != SUCCESSFUL_EXECUTION) {
					return errCode;
				}

				return SUCCESSFUL_EXECUTION;
			} else {
				return OUT_OF_MEMORY;
			}
		}
	}
}



int _find_table_cell(int segmentNumber, tableCell* tc)
{
	tc = table;

	do {
		if(tc -> segmentNumber == segmentNumber) {
			return SUCCESSFUL_EXECUTION;
		}

		tc = tc -> next;
	} while(tc != NULL);

	return INVALID_PARAMETERS;
}



int _add_new_block(segment* prevSegm, tableCell* prevTC, VA* ptr, size_t szBlock)
{
	tableCell* tc = (tableCell*) malloc(sizeof(tableCell));
	segment* segm = (segment*) malloc(sizeof(segment));

	if(tc == NULL || segm == NULL) {
		return UNKNOW_ERROR;
	}

	tc -> physAddr = ptr;
	tc -> modification = 0;
	tc -> presence = 1;
	tc -> next = NULL;
	tc -> segmentSize = szBlock;

	if(prevTC != NULL) {
		prevTC -> next = tc;
	}

	segm -> segmentNumber = tc -> segmentNumber = maxSegmentNumber++;
	segm -> next = NULL;

	if(prevSegm != NULL) {	//проверяем, первый ли это сегмент или нет
		prevSegm -> next = segm;	//если да, то ссылаемся с предыдущего сегмента на текущий
		tc -> offset = prevTC -> segmentSize;	//устанавливаем смещение текущего сегмента, как размер предыдущего
	} else {
		tc -> offset = 0;	//если нет, то устанавливаем смещение в 0
	}

	return SUCCESSFUL_EXECUTION;
}



int _find_last_segment(segment* segm) 
{
	segm = ram.head;

	while(segm != NULL) {
		if(segm -> next == NULL) {
			return SUCCESSFUL_EXECUTION;
		}

		segm = segm -> next;
	}

	return UNKNOW_ERROR;
}