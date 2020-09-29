#include "BootMgfw.h"

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
    EFI_DEVICE_PATH_PROTOCOL* BootMgfwPath;
    if (EFI_ERROR((Result = RestoreBootMgfw())))
    {
        DBG_PRINT("unable to get bootmgfw path... reason -> %r\n", Result);
        return Result;
    }

    if (EFI_ERROR((Result = InstallBootMgfwHooks(ImageHandle))))
    {
        DBG_PRINT("Failed to install bootmgfw hooks... reason -> %r\n", Result);
        return Result;
    }
    return EFI_SUCCESS;
}