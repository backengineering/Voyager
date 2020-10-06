#include "HvLoader.h"

SHITHOOK HvLoadImageHook;
SHITHOOK HvLoadImageBufferHook;
SHITHOOK HvLoadAllocImageHook;
BOOLEAN ExtendedAllocation = FALSE;
BOOLEAN HookedHyperV = FALSE;

EFI_STATUS EFIAPI HvBlImgLoadPEImageFromSourceBuffer(VOID* a1, VOID* a2, VOID* a3, VOID* a4, UINT64* ImageBase,
	UINT32* ImageSize, VOID* a7, VOID* a8, VOID* a9, VOID* a10, VOID* a11, VOID* a12, VOID* a13, VOID* a14, VOID* a15)
{
	DisableShitHook(&HvLoadImageBufferHook);
	EFI_STATUS Result = ((HV_LDR_LOAD_IMAGE_BUFFER)HvLoadImageBufferHook.Address)(a1, a2, a3, a4, ImageBase, ImageSize, a7, a8,
		a9, a10, a11, a12, a13, a14, a15);
	if(!ExtendedAllocation && !HookedHyperV)
		EnableShitHook(&HvLoadImageBufferHook);

	if (ExtendedAllocation && !HookedHyperV)
	{
		HookedHyperV = TRUE;
		EFI_IMAGE_DOS_HEADER* HypervDosHeader = *ImageBase;
		if (HypervDosHeader->e_magic != EFI_IMAGE_DOS_SIGNATURE)
			return NULL;

		EFI_IMAGE_NT_HEADERS64* HypervNtHeader = (UINT64)HypervDosHeader + HypervDosHeader->e_lfanew;
		if (HypervNtHeader->Signature != EFI_IMAGE_NT_SIGNATURE)
			return NULL;

		EFI_IMAGE_SECTION_HEADER* pSection = ((UINT64)&HypervNtHeader->OptionalHeader) +
			HypervNtHeader->FileHeader.SizeOfOptionalHeader;

		for (UINT16 i = 0; i < HypervNtHeader->FileHeader.NumberOfSections; i += 1, pSection += 1)
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

				DBG_PRINT(".reloc section base address -> 0x%p\n", *ImageBase + pSection->VirtualAddress);
				DBG_PRINT(".reloc section end (aka golden record base address) -> 0x%p\n", *ImageBase + pSection->VirtualAddress + pSection->Misc.VirtualSize);

				VOID* VmExitHook = MapModule(&VoyagerData, PayLoad);
				VOID* VmExitFunction = HookVmExit
				(
					VoyagerData.HypervModuleBase,
					VoyagerData.HypervModuleSize,
					VmExitHook
				);

				pSection->Characteristics = SECTION_RWX;
				pSection->Misc.VirtualSize += PayLoadSize();
				DBG_PRINT("VmExitHook (PayLoad Entry Point) -> 0x%p\n", VmExitHook);
			}
		}

		HypervNtHeader->OptionalHeader.SizeOfImage += PayLoadSize();
		*ImageSize += PayLoadSize();
	}

	DBG_PRINT("[HvLoader (Load Image)] ImageBase -> 0x%p, ImageSize -> 0x%p\n", *ImageBase, *ImageSize);
	return Result;
}

EFI_STATUS EFIAPI HvBlImgLoadPEImageEx(VOID* DeviceId, VOID* MemoryType, CHAR16* Path, UINT64* ImageBase, UINT32* ImageSize,
	VOID* Hash, VOID* Flags, VOID* a8, VOID* a9, VOID* a10, VOID* a11, VOID* a12, VOID* a13)
{
	DisableShitHook(&HvLoadImageHook);
	EFI_STATUS Result = ((HV_LDR_LOAD_IMAGE)HvLoadImageHook.Address)(DeviceId, MemoryType, Path, ImageBase, ImageSize, Hash, Flags, a8,
		a9, a10, a11, a12, a13);
	if(!ExtendedAllocation && !HookedHyperV)
		EnableShitHook(&HvLoadImageHook);

	if (ExtendedAllocation && !HookedHyperV)
	{
		HookedHyperV = TRUE;
		EFI_IMAGE_DOS_HEADER* HypervDosHeader = *ImageBase;
		if (HypervDosHeader->e_magic != EFI_IMAGE_DOS_SIGNATURE)
			return NULL;

		EFI_IMAGE_NT_HEADERS64* HypervNtHeader = (UINT64)HypervDosHeader + HypervDosHeader->e_lfanew;
		if (HypervNtHeader->Signature != EFI_IMAGE_NT_SIGNATURE)
			return NULL;

		EFI_IMAGE_SECTION_HEADER* pSection = ((UINT64)&HypervNtHeader->OptionalHeader) +
			HypervNtHeader->FileHeader.SizeOfOptionalHeader;

		for (UINT16 i = 0; i < HypervNtHeader->FileHeader.NumberOfSections; i += 1, pSection += 1)
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

				DBG_PRINT(".reloc section base address -> 0x%p\n", *ImageBase + pSection->VirtualAddress);
				DBG_PRINT(".reloc section end (aka golden record base address) -> 0x%p\n", *ImageBase + pSection->VirtualAddress + pSection->Misc.VirtualSize);

				VOID* VmExitHook = MapModule(&VoyagerData, PayLoad);
				VOID* VmExitFunction = HookVmExit
				(
					VoyagerData.HypervModuleBase,
					VoyagerData.HypervModuleSize,
					VmExitHook
				);

				pSection->Characteristics = SECTION_RWX;
				pSection->Misc.VirtualSize += PayLoadSize();
				DBG_PRINT("VmExitHook (PayLoad Entry Point) -> 0x%p\n", VmExitHook);
			}
		}

		HypervNtHeader->OptionalHeader.SizeOfImage += PayLoadSize();
		*ImageSize += PayLoadSize();
	}

	DBG_PRINT("[HvLoader (Load Image)] ImageBase -> 0x%p, ImageSize -> 0x%p\n", *ImageBase, *ImageSize);
	return Result;
}

UINT64 EFIAPI HvLoaderBlImgAllocateImageBuffer(VOID** imageBuffer, UINTN imageSize, UINT32 memoryType, UINT32 attributes, VOID* unused, UINT32 flags)
{
	if (imageSize >= HV_ALLOC_SIZE && !ExtendedAllocation)
	{
		ExtendedAllocation = TRUE;
		imageSize += PayLoadSize();

		// allocate the entire hyper-v module as rwx...
		memoryType = BL_MEMORY_ATTRIBUTE_RWX;
	}

	DisableShitHook(&HvLoadAllocImageHook);
	UINT64 Result = ((ALLOCATE_IMAGE_BUFFER)HvLoadAllocImageHook.Address)(imageBuffer, imageSize, memoryType, attributes, unused, flags);
	if(!ExtendedAllocation)
		EnableShitHook(&HvLoadAllocImageHook);

	DBG_PRINT("[HvLoader (Alloc Image Memory)] Allocated memory -> 0x%p, size -> 0x%x\n", *imageBuffer, imageSize);
	return Result;
}