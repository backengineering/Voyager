#pragma once
#include "TheGoldenRecord.h"

#if WINVER == 2004
#define VMEXIT_HANDLER_SIG "\x65\xC6\x04\x25\x6D\x00\x00\x00\x00\x48\x8B\x4C\x24\x00\x48\x8B\x54\x24\x00\xE8\x00\x00\x00\x00\xE9"
#define VMEXIT_HANDLER_MASK "xxxxxxxxxxxxx?xxxx?x????x"
#elif WINVER == 1909
#define VMEXIT_HANDLER_SIG "\x48\x8B\x4C\x24\x00\xEB\x07\xE8\x00\x00\x00\x00\xEB\xF2\x48\x8B\x54\x24\x00\xE8\x00\x00\x00\x00\xE9"
#define VMEXIT_HANDLER_MASK "xxxx?xxx????xxxxxx?x????x"
#elif WINVER == 1903
#define VMEXIT_HANDLER_SIG "\x48\x8B\x4C\x24\x00\xEB\x07\xE8\x00\x00\x00\x00\xEB\xF2\x48\x8B\x54\x24\x00\xE8\x00\x00\x00\x00\xE9"
#define VMEXIT_HANDLER_MASK "xxxx?xxx????xxxxxx?x????x"
#elif WINVER == 1809
#define VMEXIT_HANDLER_SIG "\x48\x8B\x4C\x24\x00\xEB\x07\xE8\x00\x00\x00\x00\xEB\xF2\x48\x8B\x54\x24\x00\xE8\x00\x00\x00\x00\xE9"
#define VMEXIT_HANDLER_MASK "xxxx?xxx????xxxxxx?x????x"
#elif WINVER == 1803
#define VMEXIT_HANDLER_SIG "\xF2\x80\x3D\xFC\x12\x46\x00\x00\x0F\x84\x00\x00\x00\x00\x48\x8B\x54\x24\x00\xE8\x00\x00\x00\x00\xE9"
#define VMEXIT_HANDLER_MASK "xxxxxxx?xx????xxxx?x????x"
#elif WINVER == 1709
#define VMEXIT_HANDLER_SIG "\xD0\x80\x3D\x78\x0A\x47\x00\x00\x0F\x84\x00\x00\x00\x00\x48\x8B\x54\x24\x00\xE8\x00\x00\x00\x00\xE9"
#define VMEXIT_HANDLER_MASK "xxxxxx??xx????xxxx?x????x"
#elif WINVER == 1703
#elif WINVER == 1607
#endif

static_assert(sizeof(VMEXIT_HANDLER_SIG) == sizeof(VMEXIT_HANDLER_MASK), "signature does not match mask size!");
static_assert(sizeof(VMEXIT_HANDLER_SIG) == 26, "signature is invalid length!");

//
// AllocBase is the base address of the extra memory allocated below where hyper-v is
// AllocSize is the size of the extra allocated memory... This size == module size...
//
VOID* MapModule(PVOYAGER_DATA_T VoyagerData, UINT8* ImageBase);

//
// sig scan hv.exe for vmexit call and replace the relative call (RVA) with 
// an RVA to the vmexit handler hook (which is the golden records entry point)...
//
// returns a pointer to the original vmexit function address...
//
VOID* HookVmExit(VOID* HypervBase, VOID* HypervSize, VOID* VmExitHook);

//
// creates a structure with all the data needed to be passed to the golden record...
//
VOID MakeVoyagerData
(
	PVOYAGER_DATA_T VoyagerData,
	VOID* HypervAlloc,
	UINT64 HypervAllocSize,
	VOID* GoldenRecordAlloc,
	UINT64 GoldenRecordSize
);