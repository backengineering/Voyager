#pragma once
#include <intrin.h>
#include <xmmintrin.h>
#include <cstddef>
#define PORT_NUM 0x2F8
#define WINVER 2004
#define DBG_PRINT(arg) \
	__outbytestring(PORT_NUM, (unsigned char*)arg, sizeof arg);

typedef struct _context_t
{
	uintptr_t rax;
	uintptr_t rcx;
	uintptr_t rdx;
	uintptr_t rbx;
	uintptr_t rsp;
	uintptr_t rbp;
	uintptr_t rsi;
	uintptr_t rdi;
	uintptr_t r8;
	uintptr_t r9;
	uintptr_t r10;
	uintptr_t r11;
	uintptr_t r12;
	uintptr_t r13;
	uintptr_t r14;
	uintptr_t r15;
	__m128 xmm0;
	__m128 xmm1;
	__m128 xmm2;
	__m128 xmm3;
	__m128 xmm4;
	__m128 xmm5;
} context_t, *pcontext_t;

#if WINVER > 1803
using vmexit_handler_t = void (__fastcall*)(pcontext_t* context, void* unknown);
#else
using vmexit_handler_t = void(__fastcall*)(pcontext_t context, void* unknown);
#endif

#pragma pack(push, 1)
typedef struct _VOYAGER_DATA_T
{
	// RVA from golden record entry ---> back to original vmexit handler...
	uintptr_t vmexit_handler_rva; 
	uintptr_t hyperv_module_base;
	uintptr_t hyperv_module_size;
	uintptr_t record_base;
	uintptr_t record_size;
} VOYAGER_DATA_T, *PVOYAGER_DATA_T;
#pragma pack(pop)
__declspec(dllexport) inline VOYAGER_DATA_T voyager_context;