#pragma once
#include <ntifs.h>


#define IO_READ	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x556, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

NTSTATUS
IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp); //Handles Communication

NTSTATUS
CloseCall(PDEVICE_OBJECT DeviceObject, PIRP Irp);

NTSTATUS
CreateCall(PDEVICE_OBJECT DeviceObject, PIRP Irp);



typedef struct _KERNEL_COPY_MEMORY_OPERATION {
	INT32 targetProcessId;
	PVOID targetAddress;
	PVOID bufferAddress;
	INT32 bufferSize;
} KERNEL_COPY_MEMORY_OPERATION, * PKERNEL_COPY_MEMORY_OPERATION;