#include "pch.h"


#define DRIVER_PREFIX "zero> "

long long g_total_read;
long long g_total_written;
FAST_MUTEX g_mutex;

void
UnloadZero(
	_In_ PDRIVER_OBJECT driver_object
);

NTSTATUS
CreateCloseZero(
	_In_ PDEVICE_OBJECT device_object,
	_In_ PIRP irp
);

NTSTATUS
ReadZero(
	_In_ PDEVICE_OBJECT device_object,
	_In_ PIRP irp
);

NTSTATUS
WriteZero(
	_In_ PDEVICE_OBJECT device_object,
	_In_ PIRP irp
);

NTSTATUS
DeviceControlZero(
	_In_ PDEVICE_OBJECT device_object,
	_In_ PIRP irp
);

NTSTATUS CompleteIRP(
	_In_ PIRP irp,
	_In_ NTSTATUS status = STATUS_SUCCESS,
	_In_ ULONG_PTR info = 0
);



EXTERN_C
NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT driver_object,
	_In_ PUNICODE_STRING registry_path
)
{
	KdPrint((DRIVER_PREFIX "loaded driver at %wZ\n", registry_path));
	driver_object->DriverUnload = UnloadZero;
	driver_object->MajorFunction[IRP_MJ_CREATE] = driver_object->MajorFunction[IRP_MJ_CLOSE] = CreateCloseZero;
	driver_object->MajorFunction[IRP_MJ_READ] = ReadZero;
	driver_object->MajorFunction[IRP_MJ_WRITE] = WriteZero;
	driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceControlZero;

	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING device_name = RTL_CONSTANT_STRING(L"\\Device\\Zero");
	UNICODE_STRING device_sym_link = RTL_CONSTANT_STRING(L"\\??\\Zero");
	ExInitializeFastMutex(&g_mutex);
	PDEVICE_OBJECT device_object = nullptr;
	do
	{
		status = IoCreateDevice(
			driver_object,
			0,
			&device_name,
			FILE_DEVICE_UNKNOWN,
			0,
			FALSE,
			&device_object
		);

		if (!NT_SUCCESS(status))
		{
			KdPrint((DRIVER_PREFIX "failed to create device (0x%08X)", status));
			break;
		}

		device_object->Flags |= DO_DIRECT_IO;
		status = IoCreateSymbolicLink(&device_sym_link, &device_name);
		if (!NT_SUCCESS(status))
		{
			KdPrint((DRIVER_PREFIX "failed to create symbolic link (0x%08X)", status));
			break;
		}
	} while (false);

	if (!NT_SUCCESS(status))
	{
		if (device_object != nullptr)
		{
			IoDeleteDevice(device_object);
		}
	}

	return status;
}

void
UnloadZero(
	_In_ PDRIVER_OBJECT driver_object
)
{
	UNREFERENCED_PARAMETER(driver_object);
	KdPrint((DRIVER_PREFIX "unloaded driver"));
}

NTSTATUS
CreateCloseZero(
	_In_ PDEVICE_OBJECT device_object,
	_In_ PIRP irp
)
{
	UNREFERENCED_PARAMETER(device_object);
	return CompleteIRP(irp);
}

NTSTATUS
ReadZero(
	_In_ PDEVICE_OBJECT device_object,
	_In_ PIRP irp
)
{

	UNREFERENCED_PARAMETER(device_object);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	ULONG um_buffer_length = stack->Parameters.Read.Length;
	
	if (um_buffer_length == 0)
	{
		return CompleteIRP(irp, STATUS_INVALID_BUFFER_SIZE);
	}
	
	NT_ASSERT(irp->MdlAddress);
	PVOID um_buffer = MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority);
	if (um_buffer == nullptr)
	{
		return CompleteIRP(irp, STATUS_INSUFFICIENT_RESOURCES);
	}
	RtlZeroMemory(um_buffer, um_buffer_length);
	ExAcquireFastMutex(&g_mutex);
	g_total_read += um_buffer_length;
	ExReleaseFastMutex(&g_mutex);
	return CompleteIRP(irp, STATUS_SUCCESS, um_buffer_length);
}

NTSTATUS
WriteZero(
	_In_ PDEVICE_OBJECT device_object,
	_In_ PIRP irp
)
{
	UNREFERENCED_PARAMETER(device_object);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	ULONG um_buffer_length = stack->Parameters.Write.Length;
	ExAcquireFastMutex(&g_mutex);
	g_total_written += um_buffer_length;
	ExReleaseFastMutex(&g_mutex);
	return CompleteIRP(irp, STATUS_SUCCESS, um_buffer_length);

}


NTSTATUS CompleteIRP(
	_In_ PIRP irp,
	_In_ NTSTATUS status,
	_In_ ULONG_PTR info
)
{
	irp->IoStatus.Status = status;
	irp->IoStatus.Information = info;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS
DeviceControlZero(
	_In_ PDEVICE_OBJECT device_object,
	_In_ PIRP irp
)
{
	UNREFERENCED_PARAMETER(device_object);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	auto& device_io_control = stack->Parameters.DeviceIoControl;
	auto status = STATUS_INVALID_DEVICE_REQUEST;
	ULONG_PTR len = 0;
	switch (device_io_control.IoControlCode)
	{
		case IOCTL_ZERO_GET_STATS:
		{
			if (device_io_control.OutputBufferLength < sizeof(ZeroStats))
			{
				status = STATUS_BUFFER_TOO_SMALL;
				break;
			}
			auto stats = (ZeroStats*)irp->AssociatedIrp.SystemBuffer;
			if (stats == nullptr)
			{
				status = STATUS_INVALID_PARAMETER;
				break;
			}
			stats->total_read = g_total_read;
			stats->total_written = g_total_written;
			len = sizeof(ZeroStats);
			status = STATUS_SUCCESS;
			break;
		}
		case IOCTL_ZERO_CLEAR_STATS:
		{
			g_total_read = g_total_written = 0;
			status = STATUS_SUCCESS;
			break;
		}
	}
	return CompleteIRP(irp, status, len);
}