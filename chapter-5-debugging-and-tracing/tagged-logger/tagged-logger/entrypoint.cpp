#include <ntddk.h>
#include "logger.h"

void
UnloadDriver(
	_In_ PDRIVER_OBJECT driver_object
)
{
	UNREFERENCED_PARAMETER(driver_object);
	Log<LogLevel::Information>("Unload called\n");
}

EXTERN_C
NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT driver_object,
	_In_ PUNICODE_STRING registry_path
)
{
	Log<LogLevel::Debug>("DriverEntry called, registry_path: %wZ\n", registry_path);
	driver_object->DriverUnload = UnloadDriver;
	return STATUS_SUCCESS;
}