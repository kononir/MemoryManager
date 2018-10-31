#ifndef MEMORYMANAGER_H

#define SUCCESSFUL_EXECUTION 0
#define INVALID_PARAMETERS -1
#define OUT_OF_BLOCK_RANGE -2
#define OUT_OF_MEMORY -2
#define UNKNOW_ERROR 1

typedef char* VA;						//тип описывающий адрес блока в ВАП
typedef char* PA;						//тип описывающий адрес блока в физической памяти

typedef struct segment					//сегмент
{
	int segmentNumber;					//номер сегмента процесса
	VA virtAddr;						//указатель на расположение в ВАП
	long segmentSize;					//размер сегмента
	long offset;						//смещение относительно начала ВАП
	struct segment* next;				//указатель на следующий сегмент
	struct segment* prev;				//указатель на предыдущий сегмент
} segment;

typedef struct virtualAddressSpace		//виртуальное адресное пространство (ВАП)
{
	VA space;							//собственно, указатель на начало ВАП
	long size;							//размер ВАП
	long ramFree;						//оставшееся свободное место ОП
	long hardFree;						//оставшееся свободное место Ж/Д
	segment* head;						//указатель на первый сегмент
	segment* tail;						//указатель на последний сегмент
} virtualAddressSpace;

typedef struct tableCell				//ячейка таблицы сегментов
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
	virtualAddressSpace vas;			//виртуальное адресное пространтсво процесса
	tableCell* head;					//указатель на начало таблицы
	tableCell* tail;					//указатель на конец таблицы
} segmentTable;

typedef struct hardSegment				//сегмент на Ж/Д
{
	int segmentNumber;					//номер сегмента
	PA data;							//расположение в памяти
	long segmentSize;					//размер
	struct hardSegment* next;			//указатель на следующий сегмент
	struct hardSegment* prev;			//указатель на предыдущий сегмент
} hardSegment;

typedef struct hardDrive
{
	hardSegment* head;					//указатель на первый сегмент
	hardSegment* tail;					//указатель на последний сегмент
} hardDrive;

typedef struct cashRecord				//запись кэша
{
	PA physAddr;						//адрес данных в основной памяти
	PA data;							//данные 
	unsigned int modification;			//бит модификации данных
	unsigned int reality;				//бит присутствия сегмента в кэше
	struct cashRecord* next;			//указатель на следующую запись
	struct cashRecord* prev;			//указатель на предыдущую запись
} cashRecord;

typedef struct cash						//кэш
{
	cashRecord* head;					//указатель на первую запись
	cashRecord* tail;					//указатель на последнюю запись
} cash;

segmentTable table;
hardDrive drive;
cash csh;

static int curSegmentNumber = 0;
static int curRecordNumber = 0;

static const int ramSize = 8192;
static const int hardSize = 65536;
static const int maxRecordNumber = 5;
static const int maxRecordSize = 10;

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

int _init_virtual_address_space(long size);
int _destroy_virtual_address_space();
int _init_cash();
int _add_cash_record();
int _destroy_cash();

int _take_free_space(VA* ptr, size_t szBlock);
int _add_new_block(segment* prevSegm, segment* nextSegm, VA* ptr, size_t szBlock);
int _add_table_cell(tableCell* prevTC, tableCell* nextTC, size_t szBlock);
int _add_segment(segment* prevSegm, segment* nextSegm, VA* ptr, size_t szBlock);
int _add_hard_segment(size_t szBlock);

int _find_segment_by_ptr(segment** segm, VA ptr);
int _find_table_cell_by_segment_number(tableCell** tc, int segmentNumber);
int _find_hard_segment_by_segment_number(hardSegment** hardSegm, int segmNumber);
int _find_cash_record_by_physical_address(cashRecord** rec, PA physAddr);
int _find_cash_record_by_current_number(cashRecord** rec);
int _find_table_cell_by_physical_address(tableCell** tc, PA physAddr);

int _free_table_cell(tableCell** tc);
int _free_segment(segment** segm);
int _free_hard_segment(hardSegment** hardSegm);
int _free_cash_record(cashRecord** rec);

int _free_space_to_load_segment(segment** segm);
int _find_cash_record_to_load_segment(cashRecord** rec, tableCell* tc);

int _load_segment_to_ram(tableCell** tc);
int _load_segment_to_hard_drive(tableCell** tc);
int _load_segment_from_cash(cashRecord** rec);
int _load_segment_to_cash(tableCell** tc, cashRecord** rec);

#endif MEMORYMANAGER_H