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