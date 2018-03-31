#include <Windows.h>
#include <cstdio>
#include "IoControlCodes.h"

DWORD64 x = 100;

void Thread()
{
	while (1)
	{
		std::printf("%lld = 0x%p\n", x, &x);
		Sleep(1000);
	}
}

int main()
{
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&Thread, NULL, NULL, NULL);
	Sleep(1000);

	HANDLE h = CreateFile(TEXT("\\\\.\\KMMM"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (h != INVALID_HANDLE_VALUE)
	{
		// GetProcessBaseAddress
		DWORD bytesIO;
		static GET_PROCESS_BASE_ADDRESS_PARAM getProcessBaseAddressParam;
		getProcessBaseAddressParam.ProcessId = GetCurrentProcessId();
		getProcessBaseAddressParam.BaseAddress = 0;

		DeviceIoControl(
			h,
			GET_PROCESS_BASE_ADDRESS_REQUEST,
			&getProcessBaseAddressParam,
			sizeof(getProcessBaseAddressParam),
			&getProcessBaseAddressParam,
			sizeof(getProcessBaseAddressParam),
			&bytesIO,
			NULL);
		std::printf("GET_PROCESS_BASE_ADDRESS_REQUEST\n");

		std::printf("value %llx\n", getProcessBaseAddressParam.BaseAddress);

		// Write
		static MEMORY_WRITE_PARAM memoryWriteParam;
		
		memoryWriteParam.ProcessId = GetCurrentProcessId();
		memoryWriteParam.Address = (DWORD64)&x;
		
		DWORD64 value = 99999999999;
		memcpy(memoryWriteParam.Value, &value, sizeof(value));
		memoryWriteParam.Size = sizeof(value);

		DeviceIoControl(
			h,
			MEMORY_WRITE_REQUEST,
			&memoryWriteParam,
			sizeof(memoryWriteParam),
			&memoryWriteParam,
			sizeof(memoryWriteParam),
			&bytesIO,
			NULL);
		std::printf("MEMORY_WRITE_REQUEST\n");

		// Read
		static MEMORY_READ_PARAM memoryReadParam;
		memoryReadParam.ProcessId = GetCurrentProcessId();
		memoryReadParam.Address = (DWORD64)&x;
		memoryReadParam.Size = sizeof(value);

		DeviceIoControl(
			h,
			MEMORY_READ_REQUEST,
			&memoryReadParam,
			sizeof(memoryReadParam),
			&memoryReadParam,
			sizeof(memoryReadParam),
			&bytesIO,
			NULL);
		std::printf("MEMORY_READ_REQUEST\n");

		memcpy(&value, memoryReadParam.Value, sizeof(value));
		std::printf("value %lld\n", value);

		CloseHandle(h);
	}

	getchar();
}