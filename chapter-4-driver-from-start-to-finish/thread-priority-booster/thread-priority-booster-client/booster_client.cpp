#include <Windows.h>
#include <stdio.h>
#include "..\thread-priority-booster-driver\booster_common.h"

int 
Error(
	const char* message
) 
{
	printf("%s (error=%u)\n", message, GetLastError());
	return 1;
}

int
main(
	int argc,
	const char* argv[]
)
{
	if (argc < 3)
	{
		printf("Usage: boost <thread_id> <priority>\n");
		return 0;
	}

	int tid = atoi(argv[1]);
	int priority = atoi(argv[2]);
	HANDLE hwnd = CreateFile(
		L"\\\\.\\Booster",
		GENERIC_WRITE,
		0,
		nullptr,
		OPEN_EXISTING,
		0,
		nullptr
	);

	if (hwnd == INVALID_HANDLE_VALUE)
		return Error("Failed to open device");

	ThreadData data;
	data.tid = tid;
	data.priority = priority;

	DWORD returned;
	BOOL success = WriteFile(
		hwnd,
		&data,
		sizeof(data),
		&returned,
		nullptr
	);

	if (!success)
		return Error("Priority change failed!");

	printf("Priority change succeded!\n");
	CloseHandle(hwnd);
	return 0;
}