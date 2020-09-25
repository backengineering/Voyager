#include "BootMgfw.h"

CHAR8* gEfiCallerBaseName = "Voyager 1";
const UINT32 _gUefiDriverRevision = 0x200;

EFI_STATUS EFIAPI UefiUnload(
    IN EFI_HANDLE ImageHandle
)
{ 
    Print(L"unloading module from memory...\n");
    return EFI_SUCCESS; 
}

EFI_STATUS EFIAPI UefiMain
(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE* SystemTable
)
{
    // get the file path to bootmgfw.efi so we can load it...
    EFI_DEVICE_PATH* BootMgfwPath = GetBootMgfwPath();
    Print(L"BootMgfwPath -> %p\n", BootMgfwPath);

    if (!BootMgfwPath)
    {
        Print(L"unable to get bootmgfw file path....\n");
        return EFI_NOT_FOUND;
    }

    EFI_STATUS Result;
    EFI_HANDLE BootMgfwHandle;

    // load bootmgfw.efi into memory...
    if (EFI_ERROR((Result = gBS->LoadImage(TRUE, ImageHandle, BootMgfwPath, NULL, 0, &BootMgfwHandle))))
    {
        Print(L"failed to load bootmgfw.efi...\n");
        return EFI_ABORTED;
    }

    Print(L"Loaded bootmgfw.efi into memory...\n");

    if (EFI_ERROR(InstallBootMgfwHooks(BootMgfwHandle)))
    {
        Print(L"Failed to install bootmgfw hooks...\n");
        return EFI_ABORTED;
    }

    Print(L"installed bootmgfw hooks...\n");

    // start bootmgfw.efi...
    if (EFI_ERROR(gBS->StartImage(BootMgfwHandle, NULL, NULL)))
    {
        Print(L"Failed to start bootmgfw.efi...\n");
        return EFI_ABORTED;
    }
    return EFI_SUCCESS;
}