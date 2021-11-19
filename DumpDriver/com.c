#include "com.h"
#include "messages.h"
#include "mem.h"
NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	Irp->IoStatus.Status = STATUS_SUCCESS; //Initialize status to STATUS_SUCESS
	Irp->IoStatus.Information = 0;         //A driver sets an IRP's I/O status block to indicate the final status of an I/O request, before calling IoCompleteRequest for the IRP.

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	//The IoCompleteRequest macro indicates that the caller has completed all processing for a given I/O request and is returning the given IRP to the I/O manager.

	DebugMessage("[TORGAST] CreateCall was called, connection enstablished!\n");

	return STATUS_SUCCESS;
}

NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	DebugMessage("[TORGAST] Connection Terminated!\n");

	return STATUS_SUCCESS;
}

NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {

	UNREFERENCED_PARAMETER(DeviceObject);

	NTSTATUS		   Status		 = STATUS_UNSUCCESSFUL;
	ULONG			   Bytes		 = NULL;
	PIO_STACK_LOCATION stack		 = IoGetCurrentIrpStackLocation(Irp);
	ULONG			   ControlCode   = stack->Parameters.DeviceIoControl.IoControlCode;

	if (ControlCode == IO_READ) {

		if (stack->Parameters.DeviceIoControl.InputBufferLength == sizeof(KERNEL_COPY_MEMORY_OPERATION)) {

			PKERNEL_COPY_MEMORY_OPERATION request = (PKERNEL_COPY_MEMORY_OPERATION)Irp->AssociatedIrp.SystemBuffer;
			PEPROCESS targetProcess;

			if (NT_SUCCESS(PsLookupProcessByProcessId(request->targetProcessId, &targetProcess)))
			{
				KeCopyVirtualMemory(targetProcess, request->targetAddress, request->bufferAddress, request->bufferSize);
				ObDereferenceObject(targetProcess);
			}

			Status = STATUS_SUCCESS;
			Bytes = sizeof(KERNEL_COPY_MEMORY_OPERATION);
			
		}
		else {
			Status = STATUS_INFO_LENGTH_MISMATCH;
			Bytes  = 0;
		}		   
	}
	else {

		Status = STATUS_INVALID_PARAMETER;
		Bytes  = 0;

	}

	Irp->IoStatus.Status      = Status;
	Irp->IoStatus.Information = Bytes;

	DebugMessage("[TORGAST] Complete request\n");
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return Status;
}