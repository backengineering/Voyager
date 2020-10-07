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

# Versions & Support

:o: -> unknown/not tested.
:heavy_check_mark: -> tested & working.
:x: -> tested & not working.

| Winver | AMD     | Intel | VM | Bare Metal |
|--------|---------|-------|----|-------|
| 2004   | :heavy_check_mark: | :heavy_check_mark:      | :heavy_check_mark:   | :heavy_check_mark:      |
| 1909   | :heavy_check_mark:        | :heavy_check_mark:      | :heavy_check_mark:   | :heavy_check_mark:      |
| 1903   | :heavy_check_mark:        | :heavy_check_mark:      | :heavy_check_mark:   | :heavy_check_mark:      |
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
Ensure voyager is renamed to `bootmgfw.efi` and `payload.dll` are all in the same folder as `launch.bat`. Simply run `launch.bat` as admin. Once it has executed, your pc will reboot.

# EFI Bundler

This code is used to bundled two EFI Modules together into a single module. The entry point of the second module is called first, then the first module's entry point is executed. Not sure what to do with this code so im just putting
it in this repo for now. Might make another project for it. 