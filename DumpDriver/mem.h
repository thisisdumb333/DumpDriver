#pragma warning (disable : 4022 4024 4047)
#pragma once

#include <ntifs.h>

NTKERNELAPI
NTSTATUS
NTAPI
MmCopyVirtualMemory(
	IN  PEPROCESS FromProcess,
	IN  CONST PVOID FromAddress,
	IN  PEPROCESS ToProcess,
	OUT PVOID ToAddress,
	IN  SIZE_T BufferSize,
	IN  KPROCESSOR_MODE PreviousMode,
	OUT PSIZE_T NumberOfBytesCopied);

NTSTATUS
KeCopyVirtualMemory(PEPROCESS Process, PVOID SourceAddres, PVOID TargetAddress, SIZE_T Size) {
	SIZE_T Bytes;
	return MmCopyVirtualMemory(Process, SourceAddres, PsGetCurrentProcess(), TargetAddress, Size, UserMode, &Bytes);
}