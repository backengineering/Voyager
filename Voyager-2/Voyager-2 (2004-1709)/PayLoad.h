#pragma once
#include "Utils.h"
extern unsigned char PayLoad[2560];

#pragma pack(push, 1)
typedef struct _VOYAGER_T
{
	UINT64 VCpuRunHandlerRVA;
	UINT64 HypervModuleBase;
	UINT64 HypervModuleSize;
	UINT64 ModuleBase;
	UINT64 ModuleSize;
} VOYAGER_T, *PVOYAGER_T;
#pragma pack(pop)

UINT32 PayLoadSize(VOID);
VOID* PayLoadEntry(VOID* ModuleBase);