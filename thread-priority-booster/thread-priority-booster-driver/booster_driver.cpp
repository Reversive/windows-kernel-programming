#include <ntifs.h>
#include "booster_common.h"
#include "logging.h"

void
UnloadDriver(
	PDRIVER_OBJECT driver_object
);

NTSTATUS
CreateClose(
	PDEVICE_OBJECT device_object,
	PIRP irp
);

NTSTATUS
Write(
	PDEVICE_OBJECT device_object,
	PIRP irp
);


EXTERN_C
NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT driver_object,
	_In_ PUNICODE_STRING registry_path
)
{
	UNREFERENCED_PARAMETER(registry_path);

	Log<LogLevel::Debug>("%s\n", "thread-priority-booster driver loaded");
	UNICODE_STRING dev_name = RTL_CONSTANT_STRING(L"\\Device\\Booster");
	PDEVICE_OBJECT device_object;
	NTSTATUS status = IoCreateDevice(
		driver_object, 
		0, 
		&dev_name, 
		FILE_DEVICE_UNKNOWN, 
		0, 
		FALSE, 
		&device_object
	);

	if (!NT_SUCCESS(status))
	{
		Log<LogLevel::Error>("failed to create device object (0x%08x)\n", status);
		return status;
	}

	UNICODE_STRING sym_link = RTL_CONSTANT_STRING(L"\\??\\Booster");
	status = IoCreateSymbolicLink(&sym_link, &dev_name);

	if (!NT_SUCCESS(status))
	{
		Log<LogLevel::Error>("failed to create symlink (0x%08x)\n", status);
		IoDeleteDevice(device_object);
		return status;
	}

	driver_object->DriverUnload = UnloadDriver;
	driver_object->MajorFunction[IRP_MJ_CREATE] = CreateClose;
	driver_object->MajorFunction[IRP_MJ_CLOSE] = CreateClose;
	driver_object->MajorFunction[IRP_MJ_WRITE] = Write;
	return STATUS_SUCCESS;
}

void
UnloadDriver(
	PDRIVER_OBJECT driver_object
)
{
	UNICODE_STRING sym_link = RTL_CONSTANT_STRING(L"\\??\\Booster");
	IoDeleteSymbolicLink(&sym_link);
	IoDeleteDevice(driver_object->DeviceObject);
	Log<LogLevel::Debug>("thread-priority-booster driver unloaded\n");
}

NTSTATUS
CreateClose(
	PDEVICE_OBJECT device_object,
	PIRP irp
)
{
	UNREFERENCED_PARAMETER(device_object);
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS
Write(
	PDEVICE_OBJECT device_object,
	PIRP irp
)
{
	UNREFERENCED_PARAMETER(device_object);
	NTSTATUS status = STATUS_SUCCESS;
	ULONG_PTR used_bytes = 0;
	PIO_STACK_LOCATION irp_sp = IoGetCurrentIrpStackLocation(irp);
	do
	{
		if (irp_sp->Parameters.Write.Length < sizeof(ThreadData))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		ThreadData* data = static_cast<ThreadData*>(irp->UserBuffer);
		if (data == nullptr || data->priority < 1 || data->priority > 31)
		{
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		PETHREAD thread;
		status = PsLookupThreadByThreadId(ULongToHandle(data->tid), &thread);
		if (!NT_SUCCESS(status))
			break;
		
		KPRIORITY old_prio = KeSetPriorityThread(thread, data->priority);
		Log<LogLevel::Debug>("Priority change for thread %u from %d to %d succeeded!\n", data->tid, old_prio, data->priority);

		ObDereferenceObject(thread);
		used_bytes = sizeof(data);
	} while (false);

	irp->IoStatus.Status = status;
	irp->IoStatus.Information = used_bytes;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;

}