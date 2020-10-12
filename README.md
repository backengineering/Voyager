<div align="center">
    <div>
        <img src="https://githacks.org/xerox/voyager/-/raw/bc07837aae126ed06130f7524e3f8464d2b3ca13/img/unknown.png"/>
    </div>
    <img src="https://githacks.org/xerox/voyager/-/raw/master/img/amd_badge.svg"/>
    <img src="https://githacks.org/xerox/voyager/-/raw/master/img/Intel-supported-green.svg"/>
    <img src="https://githacks.org/xerox/voyager/-/raw/master/img/2004--1507-supported-green.svg"/>
    <img src="https://githacks.org/xerox/voyager/-/raw/10f786da01b35d23de7abfcfecd6e54ed1efca0f/img/Secure%20Boot-Unsupported-red.svg"/>
</div>

***

### Credit

* [cr4sh](https://blog.cr4.sh/) - cr4sh has done something like this a few years back. A link to it can be found [here](https://github.com/Cr4sh/s6_pcie_microblaze/tree/master/python/payloads/DmaBackdoorHv).
* [btbd](https://github.com/btbd) - offering suggestions and providing input... also stole utils.c/utils.h :thumbsup:

# Voyager - A Hyper-V Hacking Framework For Windows 10 x64 (AMD & Intel)

Voyager is a project designed to offer module injection and vmexit hooking for both AMD & Intel versions of Hyper-V. 
This project works on all versions of Windows 10-x64 (2004-1507).

# HookChain - information and order of hooks...

<img src="https://githacks.org/xerox/voyager/-/raw/f5b9071bd896691a13466f0bd363bd16ee33e41b/img/hookchain.png"/>

***

### bootmgfw.efi

`bootmgfw.ImgArchStartBootApplication` between windows versions 2004-1709 is invoked to start winload.efi. A hook is place on this function in order to install hooks in winload.efi before
winload.efi starts execution. On windows 1703-1511 the symbol/name is different but parameters and return type are the same: `bootmgfw.BlImgStartBootApplication`.

### winload.efi

winload.efi between Windows 10-x64 versions 2004-1709 export a bunch of functions. Some of those functions are then imported by hvloader.dll such as `BlLdrLoadImage`. 
Older versions of windows 10-x64 (1703-1507) have another efi file by the name of hvloader.efi. Hvloader.efi contains alot of the same functions that are inside of winload. 
You can see that Microsoft simplified hvloader.efi in later versions of Windows 10-x64 by making winload export the functions that were also defined in hvloader.efi.

If you look at the project you will see hvloader.c/hvloader.h, these contain the hooks that are placed inside of hvloader and are installed from a hook inside of winload. 1703-1507
requires an extra set of hooks to get to where Hyper-v is loaded into memory.

### hvloader.efi

Hvloader.efi (found in windows versions 1703-1507) contains alot of the same functions that can be found inside of winload.efi as explained in the section above. In Windows 10-x64 versions spanning 1703-1507, 
Hyper-v is not loaded from a function found in winload.efi but instead of the same function found inside of hvloader.efi. These functions are `hvloader.BlImgLoadPEImageEx` 
and `hvloader.BlImgLoadPEImageFromSourceBuffer` for 1703 specifically.

### hvix64.exe (Intel)

hvix64.exe is the intel version of hyper-v. This module along with hvax64.exe does not have any symbols (no PDB). To find the vmexit handler I simply signature scanned for `0F 78` (vmread instruction)
and then xreferenced the functions that contained this instruction to see if they were called from a stub of code that pushes all registers including xmm's. It took me a little to find the correct
function but once I found the stub (vmexit handler) and c/c++ vmexit handler I was able to make a good enough signature to find the vmexit handler on all of the other Intel
versions of hyper-v.

### hvax64.exe (AMD)

hvax64.exe is the AMD version of hyper-v. This module along with hvix64.exe does not have any symbols (no PDB). To find the "vmexit handler" if you want to call it that for AMD hypervisors,
I simply signature scanned for `0f 01 d8` (VMRUN). AMD Hypervisors are basiclly a loop, first executing VMLOAD, then VMRUN (which runs the guest until an exit happens), and then
VMSAVE instruction is executed, the registers are pushed onto the stack, the exit is handled, then VMLOAD/VMRUN is executed again the cycle continues...

# Usage 

Please enable hyper-v in "turn windows features on or off". Then run launch.bat as admin, this will mount the EFI partition and move some files around then reboot you.
Voyager is designed to recover from a crash. The first thing Voyager will do when executed is restore bootmgfw on disk. If any complications occur during boot you can simply reboot.

<div align="center">
<img src="https://imgur.com/uOpcCp7.png"/>
</div>


### Compiling

Simply select the windows version you want to compile for and then compile the entire solution... Rename `Voyager.efi` to `bootmgfw.efi`, do the same for `Payload (xxx).dll`, 
rename it to `payload.dll`. Put both `bootmgfw.efi` (Voyager.efi rename), and `payload.dll` in the same folder as `launch.bat`. 

<div align="center">
    <div>
        <img src="https://githacks.org/xerox/voyager/uploads/fb3b24b28282a0cfe4c1b0440246844f/image.png"/>
    </div>
</div>