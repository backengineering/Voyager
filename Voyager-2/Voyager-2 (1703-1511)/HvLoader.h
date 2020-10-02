#pragma once
#include "Utils.h"
#include "PayLoad.h"
#include "Hvax64.h"

#if WINVER >= 1607
#define ALLOCATE_IMAGE_BUFFER_SIG "\xE8\x00\x00\x00\x00\x4C\x8B\x65\x60"
#define ALLOCATE_IMAGE_BUFFER_MASK "x????xxxx"
#elif WINVER == 1511
#define ALLOCATE_IMAGE_BUFFER_SIG "\xE8\x00\x00\x00\x00\x4C\x8B\x75\xC0"
#define ALLOCATE_IMAGE_BUFFER_MASK "x????xxxx"
#endif

#if WINVER == 1703
#define HV_LOAD_PE_IMG_SIG "\xE8\x00\x00\x00\x00\x44\x8B\xAD"
#define HV_LOAD_PE_IMG_MASK "x????xxx"
#elif WINVER <= 1607 // same for 1511
#define HV_LOAD_PE_IMG_SIG "\xE8\x00\x00\x00\x00\x48\x8B\x7D\xF7"
#define HV_LOAD_PE_IMG_MASK "x????xxxx"
#endif

static_assert(sizeof(HV_LOAD_PE_IMG_SIG) == sizeof(HV_LOAD_PE_IMG_MASK), "signature and mask do not match size...");
static_assert(sizeof(ALLOCATE_IMAGE_BUFFER_SIG) == sizeof(ALLOCATE_IMAGE_BUFFER_MASK), "signature and mask do not match size!");
typedef EFI_STATUS(EFIAPI* ALLOCATE_IMAGE_BUFFER)(VOID** imageBuffer, UINTN imageSize, UINT32 memoryType, UINT32 attributes, VOID* unused, UINT32 flags);
typedef EFI_STATUS(EFIAPI* HV_LDR_LOAD_IMAGE_BUFFER)(VOID* a1, VOID* a2, VOID* a3, VOID* a4, UINT64* ImageBase,
	UINT32* ImageSize, VOID* a7, VOID* a8, VOID* a9, VOID* a10, VOID* a11, VOID* a12, VOID* a13, VOID* a14, VOID* a15);
typedef EFI_STATUS(EFIAPI* HV_LDR_LOAD_IMAGE)(VOID* DeviceId, VOID* MemoryType, CHAR16* Path, VOID** ImageBase, UINT32* ImageSize,
	VOID* Hash, VOID* Flags, VOID* a8, VOID* a9, VOID* a10, VOID* a11, VOID* a12, VOID* a13);

UINT64 EFIAPI HvLoaderBlImgAllocateImageBuffer(VOID** imageBuffer, UINTN imageSize, UINT32 memoryType, UINT32 attributes, VOID* unused, UINT32 flags);
EFI_STATUS EFIAPI HvBlImgLoadPEImageEx(VOID* DeviceId, VOID* MemoryType, CHAR16* Path, UINT64* ImageBase, UINT32* ImageSize,
	VOID* Hash, VOID* Flags, VOID* a8, VOID* a9, VOID* a10, VOID* a11, VOID* a12, VOID* a13);
EFI_STATUS EFIAPI HvBlImgLoadPEImageFromSourceBuffer(VOID* a1, VOID* a2, VOID* a3, VOID* a4, UINT64* ImageBase,
	UINT32* ImageSize, VOID* a7, VOID* a8, VOID* a9, VOID* a10, VOID* a11, VOID* a12, VOID* a13, VOID* a14, VOID* a15);

extern SHITHOOK HvLoadImageHook;
extern SHITHOOK HvLoadAllocImageHook;
extern SHITHOOK HvLoadImageBufferHook;