#include "BootMgfw.h"
#include <Library/ShellLib.h>

CHAR8* gEfiCallerBaseName = "Voyager";
const UINT32 _gUefiDriverRevision = 0x200;

EFI_STATUS EFIAPI UefiUnload(
    IN EFI_HANDLE ImageHandle
)
{ return EFI_SUCCESS; }

EFI_STATUS EFIAPI UefiMain
(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE* SystemTable
)
{
    EFI_STATUS Result;
    EFI_HANDLE BootMgfwHandle;
    EFI_DEVICE_PATH* BootMgfwPath;

    if (EFI_ERROR((Result = GetBootMgfwPath(&BootMgfwPath))))
    {
        Print(L"unable to get bootmgfw file path... reason -> %r\n", Result);
        return EFI_NOT_FOUND;
    }

    if (EFI_ERROR((Result = gBS->LoadImage(TRUE, ImageHandle, BootMgfwPath, NULL, 0, &BootMgfwHandle))))
    {
        Print(L"failed to load bootmgfw.efi...\n");
        return EFI_ABORTED;
    }

    if (EFI_ERROR(InstallBootMgfwHooks(BootMgfwHandle)))
    {
        Print(L"Failed to install bootmgfw hooks...\n");
        return EFI_ABORTED;
    }

    if (EFI_ERROR(gBS->StartImage(BootMgfwHandle, NULL, NULL)))
    {
        Print(L"Failed to start bootmgfw.efi...\n");
        return EFI_ABORTED;
    }
    return EFI_SUCCESS;
}