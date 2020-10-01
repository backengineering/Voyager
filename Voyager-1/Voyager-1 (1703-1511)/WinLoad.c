#include "WinLoad.h"

SHITHOOK WinLoadImageShitHook;
CHAR8 ModulePathCStr[0x100];
BOOLEAN InstalledHvLoaderHook = FALSE;

EFI_STATUS EFIAPI BlImgLoadPEImageEx(VOID* a1, VOID* a2, CHAR16* ImagePath, UINT64* ImageBasePtr, UINT32* ImageSize,
	VOID* a6, VOID* a7, VOID* a8, VOID* a9, VOID* a10, VOID* a11, VOID* a12, VOID* a13, VOID* a14)
{
	UnicodeStrToAsciiStr(ImagePath, ModulePathCStr);
	DBG_PRINT("LOADING FROM WINLOAD: ");
	DBG_PRINT(ModulePathCStr);

	DisableShitHook(&WinLoadImageShitHook);
	EFI_STATUS Result = ((LDR_LOAD_IMAGE)WinLoadImageShitHook.Address)(a1, a2, ImagePath, ImageBasePtr, ImageSize, a6, a7, a8,
		a9, a10, a11, a12, a13, a14);
	if(!InstalledHvLoaderHook)
		EnableShitHook(&WinLoadImageShitHook);

	if (StrStr(ImagePath, L"hvloader.efi"))
	{
		VOID* LoadImage = 
			FindPattern(
				*ImageBasePtr, 
				*ImageSize, 
				HV_LOAD_PE_IMG_SIG,
				HV_LOAD_PE_IMG_MASK
			);

		VOID* AllocImage =
			FindPattern(
				*ImageBasePtr,
				*ImageSize,
				ALLOCATE_IMAGE_BUFFER_SIG,
				ALLOCATE_IMAGE_BUFFER_MASK
			);

		if (!LoadImage || !AllocImage)
		{
			DBG_PRINT("Signatures FAILED!\n");
			return Result;
		}

#if WINVER == 1703
		MakeShitHook(&HvLoadImageBufferHook, RESOLVE_RVA(LoadImage, 5, 1), &HvBlImgLoadPEImageFromSourceBuffer, TRUE);
#elif WINVER <= 1607 // 1511 is the same...
		MakeShitHook(&HvLoadImageHook, RESOLVE_RVA(LoadImage, 5, 1), &HvBlImgLoadPEImageEx, TRUE);
#endif

		MakeShitHook(&HvLoadAllocImageHook, RESOLVE_RVA(AllocImage, 5, 1), &HvLoaderBlImgAllocateImageBuffer, TRUE);
		DBG_PRINT("LoadImageHook -> 0x%p\n", RESOLVE_RVA(LoadImage, 5, 1));
		DBG_PRINT("AllocImage -> 0x%p\n", RESOLVE_RVA(AllocImage, 5, 1));
		InstalledHvLoaderHook = TRUE;
	}

	DBG_PRINT("[%s] Image Base -> 0x%p, Image Size -> 0x%p\n", __FUNCTION__, *ImageBasePtr, *ImageSize);
	return Result;
}