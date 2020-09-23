#pragma once
#include "Utils.h"

//
// this payload gets mapped into hyper-v's context by extending .reloc section 
// and making .reloc RWX, when hyper-v gets remapped out of winload's context
// our module will be remapped with hyper-v, linear virtual address's will change
// but RVA's will be the same... do not shithook hyper-v whilst its in winload!
//
extern unsigned char GoldenRecord[3072];

#pragma pack(push, 1)
typedef struct _VOYAGER_DATA_T
{
	UINT64 VmExitHandlerRva;
	UINT64 HypervModuleBase;
	UINT64 HypervModuleSize;
	UINT64 ModuleBase;
	UINT64 ModuleSize;
} VOYAGER_DATA_T, * PVOYAGER_DATA_T;
#pragma pack(pop)

UINT32 GetGoldenRecordSize(VOID);
VOID* GetGoldenRecordEntry(VOID* ModuleBase);