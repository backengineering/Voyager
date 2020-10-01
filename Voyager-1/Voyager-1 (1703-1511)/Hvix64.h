#pragma once
#include "PayLoad.h"
#define HV_ALLOC_SIZE 0x1400000
#define VMEXIT_HANDLER_SIG "\xD0\x80\x00\x00\x00\x00\x00\x00\x0F\x84\x00\x00\x00\x00\x48\x8B\x54\x24\x00\xE8\x00\x00\x00\x00\xE9"
#define VMEXIT_HANDLER_MASK "xx????x?xx????xxxx?x????x"

static_assert(sizeof(VMEXIT_HANDLER_SIG) == sizeof(VMEXIT_HANDLER_MASK), "signature does not match mask size!");
static_assert(sizeof(VMEXIT_HANDLER_SIG) == 26, "signature is invalid length!");

//
// AllocBase is the base address of the extra memory allocated below where hyper-v is
// AllocSize is the size of the extra allocated memory... This size == module size...
//
VOID* MapModule(pvoyager_t VoyagerData, UINT8* ImageBase);

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
	pvoyager_t VoyagerData,
	VOID* HypervAlloc,
	UINT64 HypervAllocSize,
	VOID* PayLoadBase,
	UINT64 PayLoadSize
);