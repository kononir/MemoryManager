// MemoryManager.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "stdlib.h"

typedef char* VA;						// Тип описывающий адрес блока (в ОП)

typedef struct physicalBlock
{
	long offset;
	long size;
	struct physicalBlock* next;
} physicalBlock;

typedef struct physicalMemory			//ОП
{
	VA space;							//собственно, указатель на память
	long size;
	physicalBlock* head;
} physicalMemory;

/*

typedef struct virtualSegment			//виртуальный сегмент
{
	int number;							//номер сегмента
	struct virtualSegment* next;		//указатель на следующий сегмент
} virtualSegment;

*/

typedef struct segmentsList				//список сегментов
{
	int number;							//номер виртуального сегмента (МОЖЕТ ЛУЧШЕ СДЕЛАТЬ НОМЕР ФИЗ.БЛОКА???)
	VA physAddr;						//указатель на расположение в ОП
	long segmentSize;					//размер сегмента
	unsigned int modification;			//бит модификации сегмента
	unsigned int presence;				//бит присутствия сегмента в ОП
	struct segmentsList* next;			//указатель на следующий элемент
} segmentsList;

physicalMemory physMem;
segmentsList* list;



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



int _create_table();



int _push(VA* data, size_t szPage);



int _tmain(int argc, _TCHAR* argv[])
{
	if(argc > 2){
		int n = (int)argv[1];
		int szPage = (int)argv[2];

		_init(n, szPage);

		//FILE *f = fopen("r");
	}
	
	return 0;
}



int _init (int n, int szPage)
{
	int errCode, iter;									//errCode - код ошибки

	errCode = _init_physical_memory(n, szPage);			//инициализируем ОП

														//инициализируем память под жд

														//инициализируем память под кэш

	/*errCode = _create_table();							//создаём таблицу

	for(iter = 0; iter < n && errCode == 1; iter++){	//вставляем в таблицу n полей длины szPage
		errCode = _push(NULL, szPage);
	}*/

	return errCode;
}



int _init_physical_memory(int n, int szPage)
{
	int errCode;

	int memorySize = n * szPage * sizeof(VA);
	physMem.space = (VA) malloc(memorySize);

	if(physMem.space == NULL){
		errCode = 1;
	}
	else{
		errCode = 0;
	}

	return errCode;
}



int _create_table()											//создание таблицы
{
	int errCode;

	table = (segmentTable*) malloc(sizeof(segmentTable));	//выделяем память под таблицу

	if(!table){												//проверка выделения памяти
		errCode = -1;
	}
	else{
		errCode = 1;

		table -> tableSize = 0;								//в таблице нет элементов
		table -> head = table -> tail = NULL;				//нет ни первого, ни последного элемента
	}

	return errCode;
}



int _pushTableCell(VA* dataPtr, size_t szPage)  //добавляем ячейку в конец таблицы
{
	int errCode;

	segmentTableCell *tmp = (segmentTableCell*) malloc(sizeof(segmentTableCell));	//выделяем память под ячейку таблицы

	if(!tmp){										//проверка выделения памяти
		errCode = -1;
	}
	else{
		errCode = 1;

		long virtualAddr = 

		segment* segm = (segment*) malloc(sizeof(segment));	//выделяем память под сегмент

		tmp -> segm = segm;							//присваем адрес сегмента
		tmp -> segmentSize = szPage;				//присваем размер сегмента
		tmp -> modification = 0;					//сегмент не был изменён
		tmp -> presence = 0;						//сегмент не присутствует в ОП
		tmp -> next = NULL;							//новая ячейка ссылается на нулевую ячейку
		tmp -> prev = table -> tail;				//а также на предыдую ячейку таблицы

		if(table -> head){							//если таблица не пуста
			table -> tail -> next = tmp;			//ссылаемся на новую ячейку из последней ячейки 
			table -> tail = tmp;					//и добавляем новую ячейку в конец таблицы
		}
		else{										//если таблица пуста
			table -> head = table -> tail = tmp;	//новая ячейка становится одновременно первой и последней
		}

		table -> tableSize++;						//увеличиваем рамер таблицы
	}

	return errCode;
}



int _pushSegmentBlock(long virtualAddr, VA* physAddr, virtualSegment* segm)	//добавляем блок в конец сегмента
{
	int errCode;

	segmentBlock* blc = (segmentBlock*) malloc(sizeof(segmentBlock));	//выделяем память под блок

	if(!segm){										//проверка выделения памяти
		errCode = -1;
	}
	else{
		errCode = 1;

		VA* physAddr = (VA*) malloc(sizeof(VA));	//выделяем физ. память

		blc -> virtualAddr = virtualAddr;			//присваем адрес блока сегмента
		blc -> physAddr = physAddr;					//присваем размер сегмента
		blc -> next = NULL;							//новая ячейка ссылается на нулевую ячейку
		blc -> prev = segment -> tail;				//а также на предыдую ячейку таблицы

		if(table -> head){							//если таблица не пуста
			table -> tail -> next = tmp;			//ссылаемся на новую ячейку из последней ячейки 
			table -> tail = tmp;					//и добавляем новую ячейку в конец таблицы
		}
		else{										//если таблица пуста
			table -> head = table -> tail = tmp;	//новая ячейка становится одновременно первой и последней
		}

		table -> tableSize++;						//увеличиваем рамер таблицы
	}

	return errCode;
}



int _malloc (VA* ptr, size_t szBlock)
{
	ptr = _find_free_space(szBlock);
	_add_new_block();
}



int _add_new_block()
{
	
	int errCode;

	physicalBlock* blc = phys;	//выделяем память под блок

	if(!blc){										//проверка выделения памяти
		errCode = -1;
	}
	else{
		errCode = 1;

		VA* physAddr = (VA*) malloc(sizeof(VA));	//выделяем физ. память

		blc -> virtualAddr = virtualAddr;			//присваем адрес блока сегмента
		blc -> physAddr = physAddr;					//присваем размер сегмента
		blc -> next = NULL;							//новая ячейка ссылается на нулевую ячейку
		blc -> prev = segment -> tail;				//а также на предыдую ячейку таблицы

		if(table -> head){							//если таблица не пуста
			table -> tail -> next = tmp;			//ссылаемся на новую ячейку из последней ячейки 
			table -> tail = tmp;					//и добавляем новую ячейку в конец таблицы
		}
		else{										//если таблица пуста
			table -> head = table -> tail = tmp;	//новая ячейка становится одновременно первой и последней
		}

		table -> tableSize++;						//увеличиваем рамер таблицы
	}

	return errCode;
}



VA* _find_free_space(size_t szBlock)
{
	if(physMem.space == NULL){
		return NULL;
	}
	else{
		if(physMem.head == NULL){
			return &physMem.space;
		}
		else{
			physicalBlock* curBlock = physMem.head;
			physicalBlock* nextBlock = curBlock -> next;
			int beginOfSpace, endOfSpace, space;
			bool searchingAnswer = false;

			while(curBlock -> next != NULL){
				beginOfSpace = curBlock -> offset + curBlock -> size;
				endOfSpace = nextBlock -> offset;
				space = endOfSpace - beginOfSpace;

				if(space >= szBlock){
					return (&physMem.space + beginOfSpace);
				}
			}

			space = physMem.size - beginOfSpace;

			if(space >= szBlock){
				return (&physMem.space + beginOfSpace);
			}
			else{
				return NULL;
			}
		}
	}
}