#pragma once
#include <windows.h>
#include <iostream>

#define IO_READ	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x556, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)



typedef struct _KERNEL_COPY_MEMORY_OPERATION {
	INT32 targetProcessId;
	PVOID targetAddress;
	PVOID bufferAddress;
	INT32 bufferSize;
} KERNEL_COPY_MEMORY_OPERATION, * PKERNEL_COPY_MEMORY_OPERATION;


namespace Driver {
	typedef class _KERNELDEVICE {
	private:
		HANDLE hDriver;
	public:
		_KERNELDEVICE(LPCSTR RegistryPath) {  //Need the registry path to create a handle with our driver

			hDriver = CreateFileA(RegistryPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		}

		bool Read(INT32 ProcID, PVOID targetAddress, PVOID bufferAddress, INT32 size) {

			if (hDriver == INVALID_HANDLE_VALUE) {
				printf(" Invalid Handle value!\n");
				return FALSE;
			}

			DWORD Bytes;
			KERNEL_COPY_MEMORY_OPERATION CopyRequest;

			CopyRequest.targetProcessId = ProcID;
			CopyRequest.targetAddress = targetAddress;
			CopyRequest.bufferAddress = bufferAddress;
			CopyRequest.bufferSize = size;

			if (DeviceIoControl(hDriver, IO_READ, &CopyRequest, sizeof(CopyRequest), &CopyRequest, sizeof(CopyRequest), &Bytes, NULL)) {
				return true;
			}
			return false;
		};
	}KERNELDEVICE, * PKERNELDEVICE;
}