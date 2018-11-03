#include <windows.h>
#include "SysTime.h"

long long getSysTimeInMilliseconds() {
	SYSTEMTIME time;
	
	GetSystemTime(&time);

	return time.wMilliseconds + 1000 * (time.wSecond + 60 * (time.wMinute + 60 * time.wHour));
}