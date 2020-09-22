#pragma once
#include "Utils.h"
extern unsigned char GoldenRecord[3072];

#pragma pack(push, 1)
typedef struct _VOYAGER_DATA_T
{
	VOID* VmExitHandler;
	UINT64 HypervModuleBase;
	UINT64 HypervModuleSize;
	UINT64 ModuleBase;
	UINT64 ModuleSize;
} _VOYAGER_DATA, * PVOYAGER_DATA_T;
#pragma pack(pop)

UINT32 GetGoldenRecordSize(VOID);
VOID* GetGoldenRecordEntry(VOID);