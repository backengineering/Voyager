#pragma once
#include "Utils.h"
#include "PayLoad.h"
#include "Hvix64.h"

#if WINVER >= 1607
#define ALLOCATE_IMAGE_BUFFER_SIG "\xE8\x00\x00\x00\x00\x4C\x8B\x65\x60"
#define ALLOCATE_IMAGE_BUFFER_MASK "x????xxxx"
#endif

#if WINVER == 1703
#define HV_LOAD_PE_IMG_SIG "\xE8\x00\x00\x00\x00\x44\x8B\xAD"
#define HV_LOAD_PE_IMG_MASK "x????xxx"
#elif WINVER == 1607
#define HV_LOAD_PE_IMG_SIG "\xE8\x00\x00\x00\x00\x48\x8B\x4D\x80\x41\x8B\xD4"
#define HV_LOAD_PE_IMG_MASK "x????xxxxxxx"
#endif

static_assert(sizeof(HV_LOAD_PE_IMG_SIG) == sizeof(HV_LOAD_PE_IMG_MASK), "signature and mask do not match size...");
static_assert(sizeof(ALLOCATE_IMAGE_BUFFER_SIG) == sizeof(ALLOCATE_IMAGE_BUFFER_MASK), "signature and mask do not match size!");
typedef EFI_STATUS(EFIAPI* ALLOCATE_IMAGE_BUFFER)(VOID** imageBuffer, UINTN imageSize, UINT32 memoryType, UINT32 attributes, VOID* unused, UINT32 flags);
typedef EFI_STATUS(EFIAPI* HV_LDR_LOAD_IMAGE)(VOID* a1, VOID* a2, VOID* a3, VOID* a4, UINT64* ImageBase,
	UINT32* ImageSize, VOID* a7, VOID* a8, VOID* a9, VOID* a10, VOID* a11, VOID* a12, VOID* a13, VOID* a14, VOID* a15);

UINT64 EFIAPI HvLoaderBlImgAllocateImageBuffer(VOID** imageBuffer, UINTN imageSize, UINT32 memoryType, UINT32 attributes, VOID* unused, UINT32 flags);
EFI_STATUS EFIAPI HvBlImgLoadPEImageFromSourceBuffer(VOID* a1, VOID* a2, VOID* a3, VOID* a4, UINT64* ImageBase,
	UINT32* ImageSize, VOID* a7, VOID* a8, VOID* a9, VOID* a10, VOID* a11, VOID* a12, VOID* a13, VOID* a14, VOID* a15);

extern SHITHOOK HvLoadImageHook;
extern SHITHOOK HvLoadAllocImageHook;