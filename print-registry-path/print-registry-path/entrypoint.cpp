#include <ntddk.h>

#define DTAG 'eveR'
UNICODE_STRING g_registry_path;

void 
UnloadDriver(
	_In_ PDRIVER_OBJECT driver_object
)
{
	UNREFERENCED_PARAMETER(driver_object);
	ExFreePoolWithTag(g_registry_path.Buffer, DTAG);
	KdPrint(("g_registry_path buffer released\n"));
	KdPrint(("print-registry-path driver unloaded\n"));
}



NTSTATUS
CopyRegistryPathBuffer(
	_In_ PUNICODE_STRING orig_registry_path
)
{
	g_registry_path.Buffer = (PWCH)ExAllocatePool2(POOL_FLAG_PAGED, orig_registry_path->Length, DTAG);
	KdPrint(("Allocated %d buffer bytes", orig_registry_path->Length));
	if (g_registry_path.Buffer == nullptr)
	{
		KdPrint(("Failed to allocate memory\n"));
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	g_registry_path.MaximumLength = orig_registry_path->Length;
	RtlCopyUnicodeString(&g_registry_path, orig_registry_path);
	return STATUS_SUCCESS;
}

EXTERN_C
NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT driver_object,
	_In_ PUNICODE_STRING registry_path
)
{
	KdPrint(("print-registry-path driver loaded\n"));
	driver_object->DriverUnload = UnloadDriver;
	NTSTATUS status = CopyRegistryPathBuffer(registry_path);
	if (!NT_SUCCESS(status))
	{
		return status;
	}
	KdPrint(("Original registry path: %wZ\n", registry_path));
	KdPrint(("Copied registry path: %wZ\n", &g_registry_path));
	return STATUS_SUCCESS;
}