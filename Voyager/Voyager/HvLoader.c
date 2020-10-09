#include "HvLoader.h"

SHITHOOK HvLoadImageHook;
SHITHOOK HvLoadImageBufferHook;
SHITHOOK HvLoadAllocImageHook;

BOOLEAN HvExtendedAllocation = FALSE;
BOOLEAN HvHookedHyperV = FALSE;

EFI_STATUS EFIAPI HvBlImgLoadPEImageFromSourceBuffer
(
	VOID* a1,
	VOID* a2,
	VOID* a3, 
	VOID* a4, 
	UINT64* ImageBase,
	UINT32* ImageSize, 
	VOID* a7,
	VOID* a8, 
	VOID* a9, 
	VOID* a10,
	VOID* a11,
	VOID* a12, 
	VOID* a13,
	VOID* a14,
	VOID* a15
)
{
	// disable hook and call the original...
	DisableShitHook(&HvLoadImageBufferHook);
	EFI_STATUS Result = ((HV_LDR_LOAD_IMAGE_BUFFER)HvLoadImageBufferHook.Address)
	(
		a1,
		a2,
		a3,
		a4,
		ImageBase,
		ImageSize,
		a7,
		a8,
		a9,
		a10,
		a11,
		a12,
		a13,
		a14,
		a15
	);

	// keep hooking until we have extended hyper-v allocation and hooked into hyper-v...
	if(!HvExtendedAllocation && !HvHookedHyperV)
		EnableShitHook(&HvLoadImageBufferHook);

	if (HvExtendedAllocation && !HvHookedHyperV)
	{
		HvHookedHyperV = TRUE;
		EFI_IMAGE_DOS_HEADER* HypervDosHeader = *ImageBase;
		if (HypervDosHeader->e_magic != EFI_IMAGE_DOS_SIGNATURE)
			return Result;

		EFI_IMAGE_NT_HEADERS64* HypervNtHeader = (UINT64)HypervDosHeader + HypervDosHeader->e_lfanew;
		if (HypervNtHeader->Signature != EFI_IMAGE_NT_SIGNATURE)
			return Result;

		EFI_IMAGE_SECTION_HEADER* pSection = ((UINT64)&HypervNtHeader->OptionalHeader) +
			HypervNtHeader->FileHeader.SizeOfOptionalHeader;

		for (UINT16 idx = 0; idx < HypervNtHeader->FileHeader.NumberOfSections; ++idx, ++pSection)
		{
			if (!AsciiStrCmp(&pSection->Name, ".reloc"))
			{
				VOYAGER_T VoyagerData;
				MakeVoyagerData
				(
					&VoyagerData,
					*ImageBase,
					*ImageSize,
					*ImageBase + pSection->VirtualAddress + pSection->Misc.VirtualSize,
					PayLoadSize()
				);

				HookVmExit
				(
					VoyagerData.HypervModuleBase,
					VoyagerData.HypervModuleSize,
					MapModule(&VoyagerData, PayLoad)
				);

				// make the .reloc section RWX and increase the sections size...
				pSection->Characteristics = SECTION_RWX;
				pSection->Misc.VirtualSize += PayLoadSize();
			}
		}

		// extend the size of the image in hyper-v's nt headers and LDR data entry...
		// this is required, if this is not done, then hyper-v will simply not be loaded...
		HypervNtHeader->OptionalHeader.SizeOfImage += PayLoadSize();
		*ImageSize += PayLoadSize();
	}
	return Result;
}

EFI_STATUS EFIAPI HvBlImgLoadPEImageEx
(
	VOID* DeviceId,
	VOID* MemoryType, 
	CHAR16* Path,
	UINT64* ImageBase,
	UINT32* ImageSize,
	VOID* Hash,
	VOID* Flags,
	VOID* a8,
	VOID* a9, 
	VOID* a10,
	VOID* a11, 
	VOID* a12, 
	VOID* a13
)
{
	// disable shithook and call the original...
	DisableShitHook(&HvLoadImageHook);
	EFI_STATUS Result = ((HV_LDR_LOAD_IMAGE)HvLoadImageHook.Address)
	(
		DeviceId,
		MemoryType, 
		Path, 
		ImageBase, 
		ImageSize, 
		Hash, 
		Flags,
		a8,
		a9,
		a10,
		a11, 
		a12, 
		a13
	);

	// keep hooking until we have extended hyper-v allocation and hooked into hyper-v...
	if(!HvExtendedAllocation && !HvHookedHyperV)
		EnableShitHook(&HvLoadImageHook);

	if (HvExtendedAllocation && !HvHookedHyperV)
	{
		HvHookedHyperV = TRUE;
		EFI_IMAGE_DOS_HEADER* HypervDosHeader = *ImageBase;
		if (HypervDosHeader->e_magic != EFI_IMAGE_DOS_SIGNATURE)
			return Result;

		EFI_IMAGE_NT_HEADERS64* HypervNtHeader = (UINT64)HypervDosHeader + HypervDosHeader->e_lfanew;
		if (HypervNtHeader->Signature != EFI_IMAGE_NT_SIGNATURE)
			return Result;

		EFI_IMAGE_SECTION_HEADER* pSection = ((UINT64)&HypervNtHeader->OptionalHeader) +
			HypervNtHeader->FileHeader.SizeOfOptionalHeader;

		for (UINT16 idx = 0; idx < HypervNtHeader->FileHeader.NumberOfSections; ++idx, ++pSection)
		{
			if (!AsciiStrCmp(&pSection->Name, ".reloc"))
			{
				VOYAGER_T VoyagerData;
				MakeVoyagerData
				(
					&VoyagerData,
					*ImageBase,
					*ImageSize,
					*ImageBase + pSection->VirtualAddress + pSection->Misc.VirtualSize,
					PayLoadSize()
				);

				HookVmExit
				(
					VoyagerData.HypervModuleBase,
					VoyagerData.HypervModuleSize,
					MapModule(&VoyagerData, PayLoad)
				);

				// make the .reloc section RWX and increase the sections size...
				pSection->Characteristics = SECTION_RWX;
				pSection->Misc.VirtualSize += PayLoadSize();
			}
		}

		// extend the size of the image in hyper-v's nt headers and LDR data entry...
		// this is required, if this is not done, then hyper-v will simply not be loaded...
		HypervNtHeader->OptionalHeader.SizeOfImage += PayLoadSize();
		*ImageSize += PayLoadSize();
	}
	return Result;
}

UINT64 EFIAPI HvBlImgAllocateImageBuffer
(
	VOID** imageBuffer, 
	UINTN imageSize,
	UINT32 memoryType,
	UINT32 attributes,
	VOID* unused, 
	UINT32 flags
)
{
	if (imageSize >= HV_ALLOC_SIZE && !HvExtendedAllocation)
	{
		HvExtendedAllocation = TRUE;
		imageSize += PayLoadSize();

		// allocate the entire hyper-v module as rwx...
		memoryType = BL_MEMORY_ATTRIBUTE_RWX;
	}

	// disable shithook and call the original function....
	DisableShitHook(&HvLoadAllocImageHook);
	UINT64 Result = ((ALLOCATE_IMAGE_BUFFER)HvLoadAllocImageHook.Address)
	(
		imageBuffer, 
		imageSize,
		memoryType,
		attributes,
		unused, 
		flags
	);

	// continue shithooking this function until we have extended the allocation of hyper-v...
	if(!HvExtendedAllocation)
		EnableShitHook(&HvLoadAllocImageHook);

	return Result;
}