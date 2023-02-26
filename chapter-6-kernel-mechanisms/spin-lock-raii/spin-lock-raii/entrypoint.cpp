#include <ntddk.h>
#include "spinlock.h"

void
UnloadDriver(
	_In_ PDRIVER_OBJECT driver_object
);

PKSPIN_LOCK p_spin_lock;
PKIRQL p_old_irql;

EXTERN_C
NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT driver_object,
	_In_ PUNICODE_STRING registry_path
)
{
	KdPrint(("[+] Driver loaded at %wZ\n", registry_path));
	driver_object->DriverUnload = UnloadDriver;
	p_spin_lock = static_cast<PKSPIN_LOCK>(ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(KSPIN_LOCK), 'eveR'));
	if (p_spin_lock == nullptr)
	{
		KdPrint(("[-] Error while trying to allocate spin lock\n"));
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	p_old_irql = static_cast<PKIRQL>(ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(KIRQL), 'eveR'));
	if (p_old_irql == nullptr)
	{
		KdPrint(("[-] Error while trying to allocate kirql\n"));
		ExFreePool(p_spin_lock);
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	KeInitializeSpinLock(p_spin_lock);
	SpinLock spin_lock(p_spin_lock, p_old_irql);
	{
		Locker<SpinLock> raii_spin_lock(spin_lock);
		// Using DbgPrint because KdPrint only works on PASSIVE_LEVEL
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
	if (p_spin_lock != nullptr)
	{
		ExFreePool(p_spin_lock);
		KdPrint(("[+] Released spin lock memory\n"));
	}

	if (p_old_irql != nullptr)
	{
		ExFreePool(p_old_irql);
		KdPrint(("[+] Released irql memory\n"));
	}
	KdPrint(("[+] Driver unloaded\n"));
}