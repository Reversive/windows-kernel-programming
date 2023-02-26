#pragma once
#include <wdm.h>
#include <stdio.h>
#include <stdarg.h>

#define DRIVER_PREFIX "LogDriver >"

enum class LogLevel
{
	Error = 0,
	Warning,
	Information,
	Debug,
	Verbose
};

template<LogLevel log_level>
void Log(const char* format, ...)
{
	va_list arg_list;
	va_start(arg_list, format);
	vDbgPrintExWithPrefix(
		DRIVER_PREFIX, 
		DPFLTR_IHVDRIVER_ID,
		static_cast<ULONG>(log_level), 
		format, 
		arg_list
	);
}

