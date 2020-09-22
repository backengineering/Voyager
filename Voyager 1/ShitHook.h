#pragma once
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/LoadedImage.h>
#include <IndustryStandard/PeImage.h>
#include <Guid/GlobalVariable.h>

typedef struct _SHITHOOK
{
	unsigned char Code[14];
	unsigned char JmpCode[14];

	void* Address;
	void* HookAddress;
} SHITHOOK, *PSHITHOOK;

VOID MakeShitHook(PSHITHOOK Hook, VOID* HookFrom, VOID* HookTo, BOOLEAN Install);
VOID EnableShitHook(PSHITHOOK Hook);
VOID DisableShitHook(PSHITHOOK Hook);