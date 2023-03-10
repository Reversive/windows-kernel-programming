#include <ntddk.h>

void 
HighMemoryConditionRoutine(
	_In_ PVOID highmem_event_object
)
{
	KdPrint(("[+] HighMemoryConditionRoutine thread started, waiting for signaled event...\n"));
	NTSTATUS status = KeWaitForSingleObject(
		highmem_event_object, 
		Executive, 
		KernelMode, 
		FALSE, 
		nullptr
	);
	
	if (!NT_SUCCESS(status))
	{
		KdPrint(("[-] Expected NT_SUCCESS but KeWaitForSingleObject returned 0x%d\n", status));
		return;
	}
	
	KdPrint(("[+] HighMemoryCondition event signaled!\n"));
	KdPrint(("[+] HighMemoryConditionRoutine thread finished successfuly\n"));
}

void
UnloadEventLogger(
	_In_ PDRIVER_OBJECT driver_object
)
{
	UNREFERENCED_PARAMETER(driver_object);
	KdPrint(("[+] Unloaded event logger driver\n"));
}

EXTERN_C
NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT driver_object,
	_In_ PUNICODE_STRING registry_path
)
{
	UNREFERENCED_PARAMETER(registry_path);
	driver_object->DriverUnload = UnloadEventLogger;
	UNICODE_STRING highmem_condition_name;
	RtlInitUnicodeString(&highmem_condition_name, L"\\KernelObjects\\HighMemoryCondition");
	HANDLE highmem_event_handle;
	PKEVENT hm_eobj = IoCreateNotificationEvent(&highmem_condition_name, &highmem_event_handle);
	KdPrint(("[+] HighMemoryCondition notification event created!\n"));

	HANDLE highmem_thread_handle;
	NTSTATUS status = IoCreateSystemThread(
		driver_object, 
		&highmem_thread_handle, 
		THREAD_ALL_ACCESS, 
		nullptr, 
		NtCurrentProcess(), 
		nullptr, 
		HighMemoryConditionRoutine, 
		hm_eobj
	);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("[-] Failed initializing HighMemoryCondition thread routine\n"));
		ZwClose(highmem_thread_handle);
		return status;
	}

	ZwClose(highmem_thread_handle);
	return STATUS_SUCCESS;
}