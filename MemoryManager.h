#ifndef MEMORYMANAGER_H

#define SUCCESSFUL_EXECUTION 0
#define INVALID_PARAMETERS -1
#define OUT_OF_BLOCK_RANGE -2
#define OUT_OF_MEMORY -2
#define UNKNOW_ERROR 1

typedef char* VA;						// Тип описывающий адрес блока

typedef struct tableCell
{
	int segmentNumber;					//номер сегмента процесса
	VA* physAddr;						//указатель на расположение в памяти
	long segmentSize;					//размер сегмента
	long offset;						//смещение относительно начала памяти
	unsigned int modification;			//бит модификации сегмента
	unsigned int presence;				//бит присутствия сегмента в ОП
	struct tableCell* next;				//указатель на следующую ячейку
	struct tableCell* prev;				//указатель на предыдущую ячейку
} tableCell;

typedef struct segmentTable
{
	tableCell* head;					//указатель на начало таблицы
	tableCell* tail;					//указатель на конец таблицы
} segmentTable;

typedef struct segment					//сегмент
{
	int segmentNumber;					//номер сегмента процесса
	struct segment* next;
	struct segment* prev;
} segment;

typedef struct physicalMemory			//ОП, Ж/Д, кэш
{
	VA* space;							//собственно, указатель на память
	long size;							//размер
	segment* head;						//указатель на первый сегмент
	segment* tail;						//указатель на последний сегмент
} physicalMemory;

physicalMemory ram;
physicalMemory hard;
physicalMemory cash;
segmentTable table;

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

#endif MEMORYMANAGER_H