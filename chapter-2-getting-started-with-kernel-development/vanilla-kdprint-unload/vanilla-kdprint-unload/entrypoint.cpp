#include <ntddk.h>

void 
UnloadDriver(
	_In_ PDRIVER_OBJECT driver_object
)
{
	UNREFERENCED_PARAMETER(driver_object);
	KdPrint(("Sample Driver unload function called\n"));
}

EXTERN_C
NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT driver_object,
	_In_ PUNICODE_STRING registry_path
)
{
	UNREFERENCED_PARAMETER(registry_path);
	driver_object->DriverUnload = UnloadDriver;
	KdPrint(("Sample Driver loaded\n"));
	return STATUS_SUCCESS;
}