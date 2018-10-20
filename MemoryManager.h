#ifndef MEMORYMANAGER_H

#define SUCCESSFUL_EXECUTION 0
#define INVALID_PARAMETERS -1
#define OUT_OF_BLOCK_RANGE -2
#define OUT_OF_MEMORY -2
#define UNKNOW_ERROR 1

typedef char* VA;						// Тип описывающий адрес блока в виртуальном адресном пространстве (ВАП)
typedef char* PA;						// Тип описывающий адрес блока в ОП

typedef struct tableCell
{
	int segmentNumber;					//номер сегмента процесса
	PA physAddr;						//указатель на расположение в ОП
	long segmentSize;					//размер сегмента
	unsigned int modification;			//бит модификации сегмента
	unsigned int presence;				//бит присутствия сегмента в ОП
	struct tableCell* next;				//указатель на следующую ячейку
	struct tableCell* prev;				//указатель на предыдущую ячейку
} tableCell;

typedef struct segmentTable				//таблица сегментов
{
	tableCell* head;					//указатель на начало таблицы
	tableCell* tail;					//указатель на конец таблицы
} segmentTable;

typedef struct segment					//сегмент
{
	int segmentNumber;					//номер сегмента процесса
	VA virtAddr;						//указатель на расположение в ВАП
	long segmentSize;					//размер сегмента
	long offset;						//смещение относительно начала ВАП
	struct segment* next;				//указатель на следующий сегмент
	struct segment* prev;				//указатель на предыдущий сегмент
} segment;

typedef struct virtualAddressSpace		//ОП, Ж/Д
{
	VA space;							//собственно, указатель на начало ВАП
	long ramSize;						//размер ВАП ОП
	long hardSize;						//размер ВАП Ж/Д
	long ramFree;						//оставшееся свободное место ВАП ОП
	long hardFree;						//оставшееся свободное место ВАП Ж/Д
	segment* head;						//указатель на первый сегмент
	segment* tail;						//указатель на последний сегмент
} virtualAddressSpace;

virtualAddressSpace vas;
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

int _init_virtual_address_space(virtualAddressSpace* vas, long ramSize, long hardSize);
int _destroy(virtualAddressSpace* vas);

int _take_free_space(VA* ptr, size_t szBlock);
int _add_new_block(segment* prevSegm, segment* nextSegm, VA* ptr, size_t szBlock);
int _add_table_cell(tableCell* prevTC, tableCell* nextTC, size_t szBlock);
int _add_segment(segment* prevSegm, segment* nextSegm, VA* ptr, size_t szBlock);

int _find_segment_by_ptr(segment** segm, VA ptr);
int _find_table_cell_by_segment_number(tableCell** tc, int segmentNumber);

int _free_table_cell(tableCell** tc);
int _free_segment(segment** segm);

int _free_space_to_load_segment(segment** segm);
void _load_segment_to_ram(tableCell** tc);
void _load_segment_to_hard_drive(tableCell** tc);

#endif MEMORYMANAGER_H