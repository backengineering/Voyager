#pragma once
#include "Utils.h"
extern unsigned char PayLoad[3072];

#pragma pack(push, 1)
typedef struct _voyager_t
{
	UINT64 VmExitHandlerRva;
	UINT64 HypervModuleBase;
	UINT64 HypervModuleSize;
	UINT64 ModuleBase;
	UINT64 ModuleSize;
} voyager_t, * pvoyager_t;
#pragma pack(pop)

UINT32 PayLoadSize(VOID);
VOID* PayLoadEntry(VOID* ModuleBase);