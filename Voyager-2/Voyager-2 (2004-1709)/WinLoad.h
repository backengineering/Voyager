#pragma once
#include "Utils.h"
#include "Hvax64.h"
#include "PayLoad.h"

extern SHITHOOK WinLoadImageShitHook;
extern SHITHOOK WinLoadAllocateImageHook;

#if WINVER > 1803
#define ALLOCATE_IMAGE_BUFFER_SIG "\x48\x89\x5C\x24\x10\x48\x89\x74\x24\x18\x48\x89\x7C\x24\x20\x55\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8B\xEC\x48\x83\xEC\x40\x48\x8B\x31\x4C\x8D\x7A\xFF\x45\x33\xED\x48\x89\x75"
#define ALLOCATE_IMAGE_BUFFER_MASK "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
#elif WINVER == 1803
#define ALLOCATE_IMAGE_BUFFER_SIG "\x4C\x8B\xDC\x49\x89\x5B\x00\x49\x89\x73\x00\x55\x57\x41\x54\x41\x56\x41\x57\x48\x8B\xEC"
#define ALLOCATE_IMAGE_BUFFER_MASK "xxxxxx?xxx?xxxxxxxxxxx"
#elif WINVER <= 1709
#define ALLOCATE_IMAGE_BUFFER_SIG "\x4C\x8B\xDC\x49\x89\x5B\x00\x49\x89\x73\x00\x49\x89\x7B\x00\x55\x41\x54\x41\x55\x41\x56\x41\x57"
#define ALLOCATE_IMAGE_BUFFER_MASK "xxxxxx?xxx?xxx?xxxxxxxxx"
#endif

static_assert(sizeof(ALLOCATE_IMAGE_BUFFER_SIG) == sizeof(ALLOCATE_IMAGE_BUFFER_MASK), "signature and mask do not match size!");
typedef UINT64 (EFIAPI* ALLOCATE_IMAGE_BUFFER)(VOID** imageBuffer, UINTN imageSize, UINT32 memoryType, UINT32 attributes, VOID* unused, UINT32 flags);
typedef EFI_STATUS (EFIAPI* LDR_LOAD_IMAGE)(VOID* Arg1, CHAR16* ModulePath, CHAR16* ModuleName, VOID* Arg4, VOID* Arg5, VOID* Arg6, VOID* Arg7, PPLDR_DATA_TABLE_ENTRY lplpTableEntry,
	VOID* Arg9, VOID* Arg10, VOID* Arg11, VOID* Arg12, VOID* Arg13, VOID* Arg14, VOID* Arg15, VOID* Arg16);

UINT64 EFIAPI BlImgAllocateImageBuffer(VOID** imageBuffer, UINTN imageSize, UINT32 memoryType, UINT32 attributes, VOID* unused, UINT32 flags);
EFI_STATUS EFIAPI BlLdrLoadImage(VOID* Arg1, CHAR16* ModulePath, CHAR16* ModuleName, VOID* Arg4, VOID* Arg5, VOID* Arg6, VOID* Arg7, PPLDR_DATA_TABLE_ENTRY lplpTableEntry,
	VOID* Arg9, VOID* Arg10, VOID* Arg11, VOID* Arg12, VOID* Arg13, VOID* Arg14, VOID* Arg15, VOID* Arg16);