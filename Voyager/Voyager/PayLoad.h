#pragma once
#include "Utils.h"
#include <Library/ShellLib.h>
extern VOID* PayLoad;

#pragma pack(push, 1)
typedef struct _VOYAGER_T
{
	UINT64 VmExitHandlerRva;
	UINT64 HypervModuleBase;
	UINT64 HypervModuleSize;
	UINT64 ModuleBase;
	UINT64 ModuleSize;
} VOYAGER_T, *PVOYAGER_T;
#pragma pack(pop)

#define WINDOWS_BOOTMGFW_PATH L"\\efi\\microsoft\\boot\\bootmgfw.efi"
#define PAYLOAD_PATH L"\\efi\\microsoft\\boot\\payload.dll"

UINT32 PayLoadSize(VOID);
EFI_STATUS LoadPayLoadFromDisk(VOID** PayLoadBufferPtr);
VOID* PayLoadEntry(VOID* ModuleBase);