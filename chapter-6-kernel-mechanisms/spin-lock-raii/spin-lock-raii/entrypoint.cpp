#include <ntddk.h>
#include "spinlock.h"

void
UnloadDriver(
	_In_ PDRIVER_OBJECT driver_object
);


EXTERN_C
NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT driver_object,
	_In_ PUNICODE_STRING registry_path
)
{
	KdPrint(("[+] Driver loaded at %wZ\n", registry_path));
	driver_object->DriverUnload = UnloadDriver;
	SpinLock spin_lock;
	spin_lock.Init();
	{
		Locker<SpinLock> raii_spin_lock(spin_lock);
		// Using DbgPrint to test because unicode only works on IRQL == PASSIVE_LEVEL
		DbgPrint("[+] Inside spin-lock, current irql is %d\n", KeGetCurrentIrql());
	}

	return STATUS_SUCCESS;
}


void
UnloadDriver(
	_In_ PDRIVER_OBJECT driver_object
)
{
	UNREFERENCED_PARAMETER(driver_object);
	KdPrint(("[+] Driver unloaded\n"));
}