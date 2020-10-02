#include "WinLoad.h"

SHITHOOK WinLoadImageShitHook;
SHITHOOK WinLoadAllocateImageHook;

BOOLEAN HookedHyperV = FALSE;
BOOLEAN HyperVloading = FALSE;
BOOLEAN ExtendedAllocation = FALSE;
UINT64 AllocationCount = 0;
CHAR8 ModulePathCStr[0x100];

EFI_STATUS EFIAPI BlLdrLoadImage(VOID* Arg1, CHAR16* ModulePath, CHAR16* ModuleName, VOID* Arg4, VOID* Arg5, VOID* Arg6, VOID* Arg7, PPLDR_DATA_TABLE_ENTRY lplpTableEntry,
	VOID* Arg9, VOID* Arg10, VOID* Arg11, VOID* Arg12, VOID* Arg13, VOID* Arg14, VOID* Arg15, VOID* Arg16)
{
	if (!StrCmp(ModuleName, L"hv.exe"))
		HyperVloading = TRUE;

	UnicodeStrToAsciiStr(ModuleName, ModulePathCStr);
	DBG_PRINT(ModulePathCStr);
	UnicodeStrToAsciiStr(ModulePath, ModulePathCStr);
	DBG_PRINT(ModulePathCStr);

	DisableShitHook(&WinLoadImageShitHook);
	EFI_STATUS Result = ((LDR_LOAD_IMAGE)WinLoadImageShitHook.Address)(Arg1, ModulePath, ModuleName, Arg4, Arg5, Arg6, Arg7, lplpTableEntry,
		Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16);
	// continue hooking until we hook hyper-v...
	if (!HookedHyperV)
		EnableShitHook(&WinLoadImageShitHook);

	// hv.exe has been loaded into memory...
	if (!HookedHyperV && !StrCmp(ModuleName, L"hv.exe"))
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

		for (UINT16 i = 0; i < HypervNtHeader->FileHeader.NumberOfSections; ++i, ++pSection)
		{
			if (!AsciiStrCmp(&pSection->Name, ".reloc"))
			{
				VOYAGER_T VoyagerData;
				MakeVoyagerData
				(
					&VoyagerData,
					TableEntry->ModuleBase,
					TableEntry->SizeOfImage,
					TableEntry->ModuleBase + pSection->VirtualAddress + pSection->Misc.VirtualSize,
					PayLoadSize()
				);

				DBG_PRINT(".reloc section base address -> 0x%p\n", TableEntry->ModuleBase + pSection->VirtualAddress);
				DBG_PRINT(".reloc section end (aka payload base address) -> 0x%p\n", TableEntry->ModuleBase + pSection->VirtualAddress + pSection->Misc.VirtualSize);

				VOID* VCpuRunHook = MapModule(&VoyagerData, PayLoad);
				VOID* VmExitFunction = HookVmExitHandler
				(
					VoyagerData.HypervModuleBase,
					VoyagerData.HypervModuleSize,
					VCpuRunHook
				);

				pSection->Characteristics = SECTION_RWX;
				pSection->Misc.VirtualSize += PayLoadSize();
				DBG_PRINT("vcpu_run hook (payload entry point) -> 0x%p\n", VCpuRunHook);
			}
		}

		HypervNtHeader->OptionalHeader.SizeOfImage += PayLoadSize();
		TableEntry->SizeOfImage += PayLoadSize();
	}

	DBG_PRINT("[%s] Image Base -> 0x%p, Image Size -> 0x%x\n", __FUNCTION__, (*lplpTableEntry)->ModuleBase, (*lplpTableEntry)->SizeOfImage);
	return Result;
}

UINT64 EFIAPI BlImgAllocateImageBuffer(VOID** imageBuffer, UINTN imageSize, UINT32 memoryType, UINT32 attributes, VOID* unused, UINT32 flags)
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

	DisableShitHook(&WinLoadAllocateImageHook);
	UINT64 Result = ((ALLOCATE_IMAGE_BUFFER)WinLoadAllocateImageHook.Address)(imageBuffer, imageSize, memoryType, attributes, unused, flags);
	if(!ExtendedAllocation) // keep hooking until we extend an allocation...
		EnableShitHook(&WinLoadAllocateImageHook);

	DBG_PRINT("Allocated memory -> 0x%p, size -> 0x%x\n", *imageBuffer, imageSize);
	return Result;
}