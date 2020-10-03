<div align="center">
    <div>
        <img src="https://githacks.org/xerox/voyager/-/raw/bc07837aae126ed06130f7524e3f8464d2b3ca13/img/unknown.png"/>
    </div>
    <img src="https://githacks.org/xerox/voyager/-/raw/master/img/amd_badge.svg"/>
    <img src="https://githacks.org/xerox/voyager/-/raw/master/img/Intel-supported-green.svg"/>
    <img src="https://githacks.org/xerox/voyager/-/raw/master/img/2004--1511-supported-green.svg"/>
</div>

### Credit

* [cr4sh](https://blog.cr4.sh/) - cr4sh has done something like this a few years back. A link to it can be found [here](https://github.com/Cr4sh/s6_pcie_microblaze/tree/master/python/payloads/DmaBackdoorHv).
* [btbd](https://github.com/btbd) - used utils.c/.h, also helped debug/fix issues, provided suggestions.
* [edk2](https://github.com/tianocore/edk2) - uefi library...

# Voyager - A Hyper-V Hacking Framework For Windows 10 x64 (AMD & Intel)

Voyager is a project designed to offer module injection and vmexit hooking for both AMD & Intel versions of Hyper-V. This project works on all versions of Windows 10-x64 (2004-1511).
The project is currently split into two individual projects, one for Intel and another for AMD. 

# Voyager 1 - Intel

Voyager 1 contains all the code associated with the Intel part of this project. Since intel has vmread/vmwrite instructions all that is needed is a simple hook on the vmexit handler
and interception can commence.

The payload solution contains a small CPUID interception example. I plan on expanding my examples to include EPT hooking and module injection/module shadowing. I also
need to locate the self referencing pml4e in hyper-v's pml4 :|....

# Voyager 2 - AMD

Voyager 2 contains all the code associated with the AMD part of this project. Since AMD has no vmread/vmwrite operation, only vmsave/vmload I had to locate
the linear virtual address of the VMCB for every version of windows. GS register contains a pointer to a structure defined by MS, this structure contains alot of stuff.
Deep in this structure is a linear virtual address to the current cores VMCB.

# Versions & Support

:o: -> unknown/not tested.
:heavy_check_mark: -> tested & working.
:x: -> tested & not working.

| Winver | AMD     | Intel | VM | Bare Metal |
|--------|---------|-------|----|-------|
| 2004   | :heavy_check_mark: | :heavy_check_mark:      | :heavy_check_mark:   | :heavy_check_mark:      |
| 1909   | :heavy_check_mark:        | :heavy_check_mark:      | :heavy_check_mark:   | :heavy_check_mark:      |
| 1903   | :heavy_check_mark:        | :heavy_check_mark:      | :heavy_check_mark:   | :o:      |
| 1809   | :heavy_check_mark:        | :heavy_check_mark:      | :heavy_check_mark:   | :heavy_check_mark:      |
| 1807   | :heavy_check_mark:        |  :heavy_check_mark:     | :heavy_check_mark:   | :o:      |
| 1803   | :heavy_check_mark:        | :heavy_check_mark:      | :heavy_check_mark:   | :o:      |
| 1709   | :heavy_check_mark:       | :heavy_check_mark:      | :heavy_check_mark:   |  :o:     |
| 1703   | :heavy_check_mark:        | :heavy_check_mark:      | :heavy_check_mark:   | :o:      |
| 1607   | :heavy_check_mark:        | :heavy_check_mark:      | :heavy_check_mark:   | :o:      |
| 1511   | :heavy_check_mark:        | :heavy_check_mark:      | :heavy_check_mark:   | :o:      |


# Usage

Currently the project is configured in such a way where you replace bootmgfw.efi on your EFI partition with Voyager. This requires secure boot to be disabled. If by any chance 
Voyager crashes/something goes wrong simply reboot your computer since bootmgfw.efi is restored instantly. If there are any issues please let me know!

Simply run `launch.bat` as admin. Once it has executed, your pc will reboot.

```batch
@echo off
net session >nul 2>&1
if %errorLevel% == 0 (
    :: mount the efi partition to X: drive...
    mountvol X: /S
    
    :: bootmgfw is a system file so we are going to strip those attributes away...
    attrib -s -h X:\EFI\Microsoft\Boot\bootmgfw.efi
    
    :: backup bootmgfw.efi (this is needed for voyager to work since voyager restores bootmgfw.efi)
    move X:\EFI\Microsoft\Boot\bootmgfw.efi X:\EFI\Microsoft\Boot\bootmgfw.efi.backup
    
    :: copy payload and bootmgfw.efi to EFI partition...
    xcopy %~dp0bootmgfw.efi X:\EFI\Microsoft\Boot\
    xcopy %~dp0payload.dll  X:\EFI\Microsoft\Boot\

    echo press enter to reboot...
    pause
    
    :: enable hyper-v and reboot now...
    BCDEDIT /Set {current} hypervisorlaunchtype auto
    shutdown /r /t 0
) else (
    echo Failure: Please run as admin.
    pause
)
```