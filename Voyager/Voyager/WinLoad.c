#include "WinLoad.h"

SHITHOOK WinLoadImageShitHook;
SHITHOOK WinLoadAllocateImageHook;

BOOLEAN HyperVloading = FALSE;
BOOLEAN InstalledHvLoaderHook = FALSE;

BOOLEAN ExtendedAllocation = FALSE;
BOOLEAN HookedHyperV = FALSE;
UINT64 AllocationCount = 0;

EFI_STATUS EFIAPI BlLdrLoadImage
(
	VOID* Arg1,
	CHAR16* ModulePath, 
	CHAR16* ModuleName,
	VOID* Arg4,
	VOID* Arg5, 
	VOID* Arg6, 
	VOID* Arg7, 
	PPLDR_DATA_TABLE_ENTRY lplpTableEntry,
	VOID* Arg9,
	VOID* Arg10,
	VOID* Arg11, 
	VOID* Arg12,
	VOID* Arg13,
	VOID* Arg14,
	VOID* Arg15,
	VOID* Arg16
)
{
	if (!StrCmp(ModuleName, L"hv.exe"))
		HyperVloading = TRUE;

	// disable shithook and call the original function...
	DisableShitHook(&WinLoadImageShitHook);
	EFI_STATUS Result = ((LDR_LOAD_IMAGE)WinLoadImageShitHook.Address)
	(
		Arg1,
		ModulePath, 
		ModuleName, 
		Arg4,
		Arg5,
		Arg6,
		Arg7,
		lplpTableEntry,
		Arg9,
		Arg10, 
		Arg11,
		Arg12,
		Arg13,
		Arg14,
		Arg15, 
		Arg16
	);

	// continue hooking until we inject/hook into hyper-v...
	if (!HookedHyperV)
		EnableShitHook(&WinLoadImageShitHook);

	if (StrStr(ModulePath, L"hvloader.dll"))
	{
		PLDR_DATA_TABLE_ENTRY TableEntry = *lplpTableEntry;
		VOID* HvlpTransferToHypervisor =
			FindPattern(
				TableEntry->ModuleBase,
				TableEntry->SizeOfImage,
				TRANS_TO_HV_SIG,
				TRANS_TO_HV_MASK
			);

		MakeShitHook
		(
			&TransferControlShitHook,
			RESOLVE_RVA(HvlpTransferToHypervisor, 13, 9),
			&TransferToHyperV, 
			TRUE
		);
	}

	if (!StrCmp(ModuleName, L"hv.exe"))
	{
		HookedHyperV = TRUE;
		PLDR_DATA_TABLE_ENTRY TableEntry = *lplpTableEntry;

		EFI_IMAGE_DOS_HEADER* HypervDosHeader = TableEntry->ModuleBase;
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
				//
				// the payload's base address needs to be page aligned in 
				// order for the paging table sections to be page aligned...
				//
				UINT32 PageRemainder = (0x1000 - (((TableEntry->ModuleBase + pSection->VirtualAddress + pSection->Misc.VirtualSize) << 52) >> 52));
				MakeVoyagerData
				(
					&VoyagerData,
					TableEntry->ModuleBase,
					TableEntry->SizeOfImage,
					TableEntry->ModuleBase + pSection->VirtualAddress + pSection->Misc.VirtualSize + PageRemainder,
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
		TableEntry->SizeOfImage += PayLoadSize();
	}
	return Result;
}

EFI_STATUS EFIAPI BlImgLoadPEImageEx
(
	VOID* a1,
	VOID* a2,
	CHAR16* ImagePath,
	UINT64* ImageBasePtr,
	UINT32* ImageSize,
	VOID* a6,
	VOID* a7,
	VOID* a8,
	VOID* a9, 
	VOID* a10,
	VOID* a11,
	VOID* a12,
	VOID* a13, 
	VOID* a14
)
{
	// disable shithook and call the original function...
	DisableShitHook(&WinLoadImageShitHook);
	EFI_STATUS Result = ((LDR_LOAD_IMAGE)WinLoadImageShitHook.Address)
	(
		a1,
		a2, 
		ImagePath,
		ImageBasePtr,
		ImageSize,
		a6,
		a7, 
		a8,
		a9, 
		a10, 
		a11,
		a12, 
		a13, 
		a14
	);

	// continue hooking BlImgLoadPEImageEx until we have shithooked hvloader...
	if (!InstalledHvLoaderHook)
		EnableShitHook(&WinLoadImageShitHook);

	if (StrStr(ImagePath, L"hvloader.efi"))
	{
#if WINVER == 1703
		VOID* LoadImage =
			FindPattern(
				*ImageBasePtr,
				*ImageSize,
				HV_LOAD_PE_IMG_FROM_BUFFER_SIG,
				HV_LOAD_PE_IMG_FROM_BUFFER_MASK
			);

#elif WINVER <= 1607 
		VOID* LoadImage =
			FindPattern(
				*ImageBasePtr,
				*ImageSize,
				HV_LOAD_PE_IMG_SIG,
				HV_LOAD_PE_IMG_MASK
			);

#endif
		VOID* AllocImage =
			FindPattern(
				*ImageBasePtr,
				*ImageSize,
				HV_ALLOCATE_IMAGE_BUFFER_SIG,
				HV_ALLOCATE_IMAGE_BUFFER_MASK
			);

#if WINVER == 1703
		MakeShitHook(&HvLoadImageBufferHook, RESOLVE_RVA(LoadImage, 5, 1), &HvBlImgLoadPEImageFromSourceBuffer, TRUE);
#elif WINVER <= 1607 
		MakeShitHook(&HvLoadImageHook, RESOLVE_RVA(LoadImage, 10, 6), &HvBlImgLoadPEImageEx, TRUE);
	#endif
		MakeShitHook(&HvLoadAllocImageHook, RESOLVE_RVA(AllocImage, 5, 1), &HvBlImgAllocateImageBuffer, TRUE);
		InstalledHvLoaderHook = TRUE;
	}
	return Result;
}

UINT64 EFIAPI BlImgAllocateImageBuffer
(
	VOID** imageBuffer,
	UINTN imageSize,
	UINT32 memoryType, 
	UINT32 attributes, 
	VOID* unused, 
	UINT32 Value
)
{
	//
	// The second allocation for hv.exe is used for the actual image... Wait for the second allocation before extending the allocation...
	// these allocations are not subject to change. its not a randomized or controlled order. It is what it is :|
	//
	// hv.exe
	// [BlImgAllocateImageBuffer] Alloc Base -> 0x7FFFF9FE000, Alloc Size -> 0x17C548
	// [BlImgAllocateImageBuffer] Alloc Base -> 0xFFFFF80608120000, Alloc Size -> 0x1600000
	// [BlImgAllocateImageBuffer] Alloc Base -> 0xFFFFF80606D68000, Alloc Size -> 0x2148
	// [BlLdrLoadImage] Image Base -> 0xFFFFF80608120000, Image Size -> 0x1600000
	//

	if (HyperVloading && !ExtendedAllocation && ++AllocationCount == 2)
	{
		ExtendedAllocation = TRUE;
		imageSize += PayLoadSize();

		// allocate the entire hyper-v module as rwx...
		memoryType = BL_MEMORY_ATTRIBUTE_RWX;
	}
	
	// disable shithook and call the original function...
	DisableShitHook(&WinLoadAllocateImageHook);
	UINT64 Result = ((ALLOCATE_IMAGE_BUFFER)WinLoadAllocateImageHook.Address)
	(
		imageBuffer,
		imageSize,
		memoryType,
		attributes, 
		unused,
		Value
	);

	// keep hooking until we extend an allocation...
	if(!ExtendedAllocation)
		EnableShitHook(&WinLoadAllocateImageHook);

	return Result;
}