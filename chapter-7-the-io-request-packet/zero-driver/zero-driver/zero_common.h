#pragma once
#include <ntddk.h>
#define DEVICE_ZERO 0x8022
#define IOCTL_ZERO_GET_STATS \
CTL_CODE(DEVICE_ZERO, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_ZERO_CLEAR_STATS \
CTL_CODE(DEVICE_ZERO, 0x801, METHOD_NEITHER, FILE_ANY_ACCESS)

struct ZeroStats
{
	long long total_read;
	long long total_written;
};