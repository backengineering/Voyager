#pragma once
#include "Utils.h"
#include "HvLoader.h"
#include "PayLoad.h"

extern SHITHOOK WinLoadImageShitHook;

#if WINVER == 1703
#define LOAD_PE_IMG_SIG "\xE8\x00\x00\x00\x00\x85\xC0\x79\x45"
#define LOAD_PE_IMG_MASK "x????xxxx"
#elif WINVER == 1607
#define LOAD_PE_IMG_SIG "\xE8\x00\x00\x00\x00\x48\x8B\x7D\xF7"
#define LOAD_PE_IMG_MASK "x????xxxx"
#endif

static_assert(sizeof(LOAD_PE_IMG_SIG) == sizeof(LOAD_PE_IMG_MASK), "signature and mask do not match size...");
typedef EFI_STATUS (EFIAPI* LDR_LOAD_IMAGE)(VOID* a1, VOID* a2, CHAR16* ImagePath, UINT64* ImageBasePtr, UINT32* ImageSize,
	VOID* a6, VOID* a7, VOID* a8, VOID* a9, VOID* a10, VOID* a11, VOID* a12, VOID* a13, VOID* a14);

EFI_STATUS EFIAPI BlImgLoadPEImageEx(VOID* a1, VOID* a2, CHAR16* ImagePath, UINT64* ImageBasePtr, UINT32* ImageSize,
	VOID* a6, VOID* a7, VOID* a8, VOID* a9, VOID* a10, VOID* a11, VOID* a12, VOID* a13, VOID* a14);