#include <ntddk.h>

void
UnloadDriver(
	_In_ PDRIVER_OBJECT driver_object
)
{
	KdPrint(("os-version-driver unloaded\n"));
	UNREFERENCED_PARAMETER(driver_object);
}

void
PrintOsVersion()
{
	RTL_OSVERSIONINFOW version_information;
	version_information.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
	NTSTATUS status = RtlGetVersion(&version_information);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Unexpected failure on RtlGetVersion call: %d\n", status));
		return;
	}

	KdPrint(
		(
			"Major version: %d, Minor version: %d, Build number: %d",
			version_information.dwMajorVersion,
			version_information.dwMinorVersion,
			version_information.dwBuildNumber
			)
	);
}

EXTERN_C
NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT driver_object,
	_In_ PUNICODE_STRING registry_path
)
{
	KdPrint(("os-version-driver loaded\n"));
	UNREFERENCED_PARAMETER(registry_path);
	driver_object->DriverUnload = UnloadDriver;
	PrintOsVersion();
	return STATUS_SUCCESS;
}