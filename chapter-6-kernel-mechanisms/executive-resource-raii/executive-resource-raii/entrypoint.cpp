#include <ntddk.h>
#include "resource.h"

#define DTAG 'eveR'

void
UnloadDriver(
	_In_ PDRIVER_OBJECT driver_object
);

PERESOURCE resource;

EXTERN_C
NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT driver_object,
	_In_ PUNICODE_STRING registry_path
)
{
	KdPrint(("[+] Driver loaded, registry_path: %wZ\n", registry_path));
	driver_object->DriverUnload = UnloadDriver;
	resource = (PERESOURCE)ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(ERESOURCE), DTAG);
	if (resource == nullptr)
	{
		KdPrint(("[-] Not enough memory to allocate the resource\n"));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	NTSTATUS status = ExInitializeResourceLite(resource);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("[-] Error while trying to initialize the resource, returned (0x%d)\n", status));
		ExFreePool(resource);
		return status;
	}
	KdPrint(("[+] Resource allocated successfully\n"));

	SharedResource shared_resource(resource);
	ExclusiveResource exclusive_resource(resource);
	{
		Resource<ExclusiveResource> exclusive_resource_raii(&exclusive_resource);
		Resource<SharedResource> shared_resource_raii(&shared_resource);
	}

	ASSERT(status == STATUS_SUCCESS);
	return STATUS_SUCCESS;
}

void
UnloadDriver(
	_In_ PDRIVER_OBJECT driver_object
)
{
	UNREFERENCED_PARAMETER(driver_object);
	if(resource != nullptr)
	{
		ExDeleteResourceLite(resource);
		ExFreePool(resource);
	}
	KdPrint(("[+] Unloaded driver successfuly\n"));
}