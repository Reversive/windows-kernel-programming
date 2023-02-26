#pragma once
#include <stdarg.h>
#include <wdm.h>

#define DRIVER_PREFIX "Booster > "

enum class LogLevel
{
	Error = 0,
	Warning,
	Information,
	Debug,
	Verbose
};


template<LogLevel level>
NTSTATUS Log(PCSTR format, ...)
{
	NTSTATUS status = STATUS_SUCCESS;
	va_list list;
	va_start(list, format);
	status = vDbgPrintExWithPrefix(DRIVER_PREFIX, DPFLTR_IHVDRIVER_ID, static_cast<ULONG>(LogLevel::Error), format, list);
	va_end(list);
	return status;
}
