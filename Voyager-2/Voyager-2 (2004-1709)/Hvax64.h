#pragma once
#include "PayLoad.h"

#if WINVER == 2004
#define VCPU_RUN_HANDLER_SIG "\xE8\x00\x00\x00\x00\x0F\x01\xDC"
#define VCPU_RUN_HANDLER_MASK "x????xxx"
#endif

static_assert(sizeof(VCPU_RUN_HANDLER_SIG) == sizeof(VCPU_RUN_HANDLER_MASK), "signature does not match mask size!");

//
// AllocBase is the base address of the extra memory allocated below where hyper-v is
// AllocSize is the size of the extra allocated memory... This size == module size...
//
VOID* MapModule(PVOYAGER_T VoyagerData, UINT8* ImageBase);

//
// sig scan hv.exe for vmexit call and replace the relative call (RVA) with 
// an RVA to the vmexit handler hook (which is the golden records entry point)...
//
// returns a pointer to the original vmexit function address...
//
VOID* HookVCpuRun(VOID* HypervBase, VOID* HypervSize, VOID* VmExitHook);

//
// creates a structure with all the data needed to be passed to the golden record...
//
VOID MakeVoyagerData
(
	PVOYAGER_T VoyagerData,
	VOID* HypervAlloc,
	UINT64 HypervAllocSize,
	VOID* PayLoadBase,
	UINT64 PayLoadSize
);