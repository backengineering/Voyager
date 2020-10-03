#include "BootMgfw.h"
#include "SplashScreen.h"

CHAR8* gEfiCallerBaseName = "Voyager 1";
const UINT32 _gUefiDriverRevision = 0x200;

EFI_STATUS EFIAPI UefiUnload(EFI_HANDLE ImageHandle)
{
    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI UefiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
    EFI_STATUS Result;
    EFI_HANDLE BootMgfwHandle;

    gST->ConOut->ClearScreen(gST->ConOut);
    gST->ConOut->OutputString(gST->ConOut, AsciiArt);
    Print(L"\n");

    // since we replaced bootmgfw on disk, we are going to need to restore the image back
    // this is simply just moving bootmgfw.efi.backup to bootmgfw.efi...
    if (EFI_ERROR((Result = RestoreBootMgfw())))
    {
        Print(L"unable to restore bootmgfw... reason -> %r\n", Result);
        gBS->Stall(5 * 1000000);
        return Result;
    }

    // the payload is sitting on disk... we are going to load it into memory...
    if (EFI_ERROR((Result = LoadPayLoadFromDisk(&PayLoad))))
    {
        Print(L"failed to read payload from disk... reason -> %r\n", Result);
        gBS->Stall(5 * 1000000);
        return Result;
    }

    EFI_DEVICE_PATH* BootMgfwPath = GetBootMgfwPath();
    if (EFI_ERROR((Result = gBS->LoadImage(TRUE, ImageHandle, BootMgfwPath, NULL, NULL, &BootMgfwHandle))))
    {
        Print(L"failed to load bootmgfw.efi... reason -> %r\n", Result);
        gBS->Stall(5 * 1000000);
        return EFI_ABORTED;
    }

    if (EFI_ERROR((Result = InstallBootMgfwHooks(BootMgfwHandle))))
    {
        Print(L"Failed to install bootmgfw hooks... reason -> %r\n", Result);
        gBS->Stall(5 * 1000000);
        return Result;
    }

    if (EFI_ERROR((Result = gBS->StartImage(BootMgfwHandle, NULL, NULL))))
    {
        Print(L"Failed to start bootmgfw.efi... reason -> %r\n", Result);
        gBS->Stall(5 * 1000000);
        return EFI_ABORTED;
    }

    gBS->Stall(5 * 1000000);
    return EFI_SUCCESS;
}