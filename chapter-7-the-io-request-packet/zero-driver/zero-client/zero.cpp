#include <Windows.h>
#include <stdio.h>

int Error(const char* msg)
{
	printf("%s: error=%u\n", msg, ::GetLastError());
	return 1;
}

int
main(
	int argc,
	const char* argv[]
)
{
	HANDLE device_handle = ::CreateFile(
		L"\\\\.\\Zero",
		GENERIC_READ | GENERIC_WRITE,
		0,
		nullptr,
		OPEN_EXISTING,
		0,
		nullptr
	);
	if (device_handle == INVALID_HANDLE_VALUE)
	{
		return Error("failed to open device");
	}
	
	BYTE buffer[64];
	for (int i = 0; i < sizeof(buffer); i++)
	{
		buffer[i] = i + 1;
	}

	DWORD bytes;
	BOOL ok = ReadFile(
		device_handle,
		buffer,
		sizeof(buffer),
		&bytes,
		nullptr
	);

	if (!ok)
	{
		return Error("failed to read");
	}

	if (bytes != sizeof(buffer))
	{
		printf("wrong number of bytes");
	}

	for (auto n : buffer)
	{
		if (n != 0)
		{
			printf("wrong data\n");
			break;
		}
	}

	BYTE buffer2[1024];
	ok = WriteFile(
		device_handle,
		buffer2, 
		sizeof(buffer2), 
		&bytes, 
		nullptr
	);

	if (!ok)
	{
		return Error("failed to write");
	}

	if (bytes != sizeof(buffer2))
	{
		printf("wrong byte count\n");
	}
	CloseHandle(device_handle);
	return 0;
}