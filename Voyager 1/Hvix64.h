#pragma once
#include "TheGoldenRecord.h"
#define VMEXIT_HANDLER "\x65\xC6\x04\x25\x6D\x00\x00\x00\x00\x48\x8B\x4C\x24\x20\x48\x8B\x54\x24\x30\xE8\x6B\xBF\xFE\xFF\xE9"

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