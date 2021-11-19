#pragma once
#include <ntifs.h>
#include "messages.h"
#include "com.h"

NTSTATUS
DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath);

NTSTATUS
UnloadDriver(PDRIVER_OBJECT pDriverObject);

PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING dev, dos;