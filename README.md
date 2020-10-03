<div align="center">
    <div>
        <img src="https://githacks.org/xerox/voyager/-/raw/bc07837aae126ed06130f7524e3f8464d2b3ca13/img/unknown.png"/>
    </div>
    <img src="https://githacks.org/xerox/voyager/-/raw/master/img/amd_badge.svg"/>
    <img src="https://githacks.org/xerox/voyager/-/raw/master/img/Intel-supported-green.svg"/>
    <img src="https://githacks.org/xerox/voyager/-/raw/master/img/2004--1511-supported-green.svg"/>
</div>

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

The payload for AMD is also just a cpuid interception example. 

```
#if WINVER == 2004
#define offset_vmcb_base 0x103B0
#define offset_vmcb_link 0x198
#define offset_vmcb 0xE80
#elif WINVER == 1909
#define offset_vmcb_base 0x83B0
#define offset_vmcb_link 0x190
#define offset_vmcb 0xD00
#elif WINVER == 1903
#define offset_vmcb_base 0x83B0
#define offset_vmcb_link 0x190
#define offset_vmcb 0xD00
#elif WINVER == 1809
#define offset_vmcb_base 0x83B0
#define offset_vmcb_link 0x198
#define offset_vmcb 0xD00
#elif WINVER == 1803
#define offset_vmcb_base 0x82F0
#define offset_vmcb_link 0x168
#define offset_vmcb 0xCC0
#elif WINVER == 1709
#define offset_vmcb_base 0x82F0
#define offset_vmcb_link 0x88
#define offset_vmcb 0xC80
#elif WINVER == 1703
#define offset_vmcb_base 0x82F0
#define offset_vmcb_link 0x80
#define offset_vmcb 0xBC0
#elif WINVER == 1607
#define offset_vmcb_base 0x82F0
#define offset_vmcb_link 0x90
#define offset_vmcb 0xBC0
#elif WINVER == 1511
#define offset_vmcb_base 0x82F0
#define offset_vmcb_link 0x90
#define offset_vmcb 0xC40
#endif
```

Ill probably end up sig scanning for these offsets/resolving them at runtime when i condense this project down to a single solution.

# Versions & Support

:o: -> unknown/not tested.
:heavy_check_mark: -> tested & working.
:x: -> tested & not working.

| Winver | AMD     | Intel | VM | Bare Metal |
|--------|---------|-------|----|-------|
| 2004   | :heavy_check_mark: | :heavy_check_mark:      | :heavy_check_mark:   | :heavy_check_mark:      |
| 1909   | :heavy_check_mark:        | :heavy_check_mark:      | :heavy_check_mark:   | :o:      |
| 1903   | :heavy_check_mark:        | :heavy_check_mark:      | :heavy_check_mark:   | :o:      |
| 1809   | :heavy_check_mark:        | :heavy_check_mark:      | :heavy_check_mark:   | :o:      |
| 1807   | :heavy_check_mark:        |  :heavy_check_mark:     | :heavy_check_mark:   | :o:      |
| 1803   | :heavy_check_mark:        | :heavy_check_mark:      | :heavy_check_mark:   | :o:      |
| 1709   | :heavy_check_mark:       | :heavy_check_mark:      | :heavy_check_mark:   |  :o:     |
| 1703   | :heavy_check_mark:        | :heavy_check_mark:      | :heavy_check_mark:   | :o:      |
| 1607   | :heavy_check_mark:        | :heavy_check_mark:      | :heavy_check_mark:   | :o:      |
| 1511   | :heavy_check_mark:        | :heavy_check_mark:      | :heavy_check_mark:   | :o:      |