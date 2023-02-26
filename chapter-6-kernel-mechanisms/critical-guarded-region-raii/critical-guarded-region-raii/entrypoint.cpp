#include "region.h"

void
UnloadDriver(
	_In_ PDRIVER_OBJECT driver_object
)
{
	UNREFERENCED_PARAMETER(driver_object);
	KdPrint(("[+] Driver unloaded successfuly\n"));
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
	CriticalRegion critical_region;
	GuardedRegion guarded_region;
	{
		Region<CriticalRegion> raii_critical_region(&critical_region);
		Region<GuardedRegion> raii_guarded_region(&guarded_region);
	}
	KdPrint(("[+] Driver loaded successfuly\n"));
	return STATUS_SUCCESS;
}