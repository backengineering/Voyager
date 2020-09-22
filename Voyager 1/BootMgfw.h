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
#include <Guid/GlobalVariable.h>
#include "WinLoad.h"

#define START_BOOT_APPLICATION "\x48\x8B\xC4\x48\x89\x58\x20\x44\x89\x40\x18\x48\x89\x50\x10\x48\x89\x48\x08\x55\x56\x57\x41\x54"
#define WINDOWS_BOOTMGR_PATH L"\\efi\\microsoft\\boot\\bootmgfw.efi"

extern SHITHOOK BootMgfwShitHook;
typedef EFI_STATUS(EFIAPI* IMG_ARCH_START_BOOT_APPLICATION)(VOID*, VOID*, UINT32, UINT8, VOID*);
EFI_DEVICE_PATH* EFIAPI GetBootMgfwPath(VOID);
EFI_STATUS EFIAPI InstallBootMgfwHooks(EFI_HANDLE BootMgfwPath);
EFI_STATUS EFIAPI ArchStartBootApplicationHook(VOID* AppEntry, VOID* ImageBase, UINT32 ImageSize, UINT8 BootOption, VOID* ReturnArgs);