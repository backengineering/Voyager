#pragma once
#include "Utils.h"

typedef struct _SHITHOOK
{
	unsigned char Code[14];
	unsigned char JmpCode[14];

	void* Address;
	void* HookAddress;
} SHITHOOK, *PSHITHOOK;

VOID MakeShitHook(PSHITHOOK Hook, VOID* HookFrom, VOID* HookTo, BOOLEAN Install);
VOID EnableShitHook(PSHITHOOK Hook);
VOID DisableShitHook(PSHITHOOK Hook);