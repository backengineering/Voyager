#include "BootMgfw.h"
#include "SplashScreen.h"

CHAR8* gEfiCallerBaseName = "Voyager 2";
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

    gST->ConOut->ClearScreen(gST->ConOut);
    gST->ConOut->OutputString(gST->ConOut, AsciiArt);
    Print(L"\n");

    if (EFI_ERROR((Result = RestoreBootMgfw())))
    {
        Print(L"unable to restore bootmgfw... reason -> %r\n", Result);
        return Result;
    }

    if (EFI_ERROR((Result = InstallBootMgfwHooks(ImageHandle))))
    {
        Print(L"Failed to install bootmgfw hooks... reason -> %r\n", Result);
        return Result;
    }

    gBS->Stall(5 * 1000000);
    return EFI_SUCCESS;
}