#include <Windows.h>
#include <winternl.h>
#include <stdio.h>
#include <ntddbeep.h>
#include <array>
#pragma comment (lib, "ntdll")

void
PlaySound(
	HANDLE hwnd,
	ULONG dur,
	ULONG freq
)
{
	BEEP_SET_PARAMETERS bsp;
	bsp.Duration = dur;
	bsp.Frequency = freq;
	DWORD bytes;
	::DeviceIoControl(
		hwnd,
		IOCTL_BEEP_SET,
		&bsp,
		sizeof(bsp),
		nullptr,
		0,
		&bytes,
		nullptr
	);
	::Sleep(dur);
}

int 
main(
	int argc, 
	const char* argv[]
)
{
	HANDLE hwnd;
	OBJECT_ATTRIBUTES fo_attributes;
	UNICODE_STRING name;
	RtlInitUnicodeString(&name, DD_BEEP_DEVICE_NAME_U);
	InitializeObjectAttributes(&fo_attributes, &name, OBJ_CASE_INSENSITIVE, nullptr, nullptr);
	IO_STATUS_BLOCK io_status;
	NTSTATUS status = ::NtOpenFile(
		&hwnd,
		GENERIC_WRITE,
		&fo_attributes,
		&io_status,
		0,
		0);
	if (!NT_SUCCESS(status))
	{
		printf("Could not open beep device\n");
		return FALSE;
	}
	for (auto [freq, dur] : std::array<std::pair<ULONG, ULONG>, 4>{ { { 1000, 1000 }, { 2000, 1000 }, { 3000, 1000 }, { 4000, 1000 } } })
	{
		PlaySound(hwnd, dur, freq);
	}
	::CloseHandle(hwnd);
	return TRUE;
}