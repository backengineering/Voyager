#pragma once
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/LoadedImage.h>
#include <IndustryStandard/PeImage.h>
#include <Library/ShellLib.h>
#include <Guid/GlobalVariable.h>
#include "WinLoad.h"

#define START_BOOT_APPLICATION_SIG "\xE8\x00\x00\x00\x00\x48\x8B\xCE\x8B\xD8\xE8\x00\x00\x00\x00\x41\x8B\xCF"
#define START_BOOT_APPLICATION_MASK "x????xxxxxx????xxx"

static_assert(sizeof(START_BOOT_APPLICATION_SIG) == sizeof(START_BOOT_APPLICATION_MASK), "signature and mask size's dont match...");
#define WINDOWS_BOOTMGR_PATH L"\\efi\\microsoft\\boot\\bootmgfw.efi"
extern SHITHOOK BootMgfwShitHook;
typedef EFI_STATUS(EFIAPI* IMG_ARCH_START_BOOT_APPLICATION)(VOID*, VOID*, UINT32, UINT8, VOID*);
EFI_STATUS EFIAPI GetBootMgfwPath(EFI_DEVICE_PATH_PROTOCOL** BootMgfwPathProtocol);
EFI_STATUS EFIAPI InstallBootMgfwHooks(EFI_HANDLE ImageHandle);
EFI_STATUS EFIAPI ArchStartBootApplicationHook(VOID* AppEntry, VOID* ImageBase, UINT32 ImageSize, UINT8 BootOption, VOID* ReturnArgs);