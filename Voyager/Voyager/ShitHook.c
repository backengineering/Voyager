#include "ShitHook.h"

VOID MakeShitHook(PSHITHOOK Hook, VOID* HookFrom, VOID* HookTo, BOOLEAN Install)
{
	if (!Hook || !HookFrom || !HookTo)
		return;

	unsigned char JmpCode[14] =
	{
		0xff, 0x25, 0x0, 0x0, 0x0, 0x0,		// jmp    QWORD PTR[rip + 0x0]

		// jmp address...
		0x0, 0x0, 0x0, 0x0,
		0x0, 0x0, 0x0, 0x0
	};

	// save original bytes, and hook related addresses....
	Hook->Address = HookFrom;
	Hook->HookAddress = HookTo;
	gBS->CopyMem(Hook->Code, HookFrom, sizeof Hook->Code);

	// setup hook...
	gBS->CopyMem(JmpCode + 6, &HookTo, sizeof HookTo);
	gBS->CopyMem(Hook->JmpCode, JmpCode, sizeof JmpCode);
	if (Install) EnableShitHook(Hook);
}

VOID EnableShitHook(PSHITHOOK Hook)
{
	gBS->CopyMem(Hook->Address, Hook->JmpCode, sizeof Hook->JmpCode);
}

VOID DisableShitHook(PSHITHOOK Hook)
{
	gBS->CopyMem(Hook->Address, Hook->Code, sizeof Hook->Code);
}