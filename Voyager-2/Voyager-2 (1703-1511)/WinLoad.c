#include "WinLoad.h"

SHITHOOK WinLoadImageShitHook;
BOOLEAN InstalledHvLoaderHook = FALSE;

EFI_STATUS EFIAPI BlImgLoadPEImageEx(VOID* a1, VOID* a2, CHAR16* ImagePath, UINT64* ImageBasePtr, UINT32* ImageSize,
	VOID* a6, VOID* a7, VOID* a8, VOID* a9, VOID* a10, VOID* a11, VOID* a12, VOID* a13, VOID* a14)
{
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

#if WINVER == 1703
		MakeShitHook(&HvLoadImageBufferHook, RESOLVE_RVA(LoadImage, 5, 1), &HvBlImgLoadPEImageFromSourceBuffer, TRUE);
#elif WINVER <= 1607 
		MakeShitHook(&HvLoadImageHook, RESOLVE_RVA(LoadImage, 5, 1), &HvBlImgLoadPEImageEx, TRUE);
#endif

		MakeShitHook(&HvLoadAllocImageHook, RESOLVE_RVA(AllocImage, 5, 1), &HvLoaderBlImgAllocateImageBuffer, TRUE);
		InstalledHvLoaderHook = TRUE;
	}
	return Result;
}