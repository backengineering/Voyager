#pragma once
#include <intrin.h>
#include <xmmintrin.h>
#include <cstddef>

#define PORT_NUM 0x2F8
#define WINVER 1511
#define DBG_PRINT(arg) \
	__outbytestring(PORT_NUM, (unsigned char*)arg, sizeof arg);

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
using u128 = __m128;

typedef struct _context_t
{
	u64 rax;
	u64 rcx;
	u64 rdx;
	u64 rbx;
	u64 rsp;
	u64 rbp;
	u64 rsi;
	u64 rdi;
	u64 r8;
	u64 r9;
	u64 r10;
	u64 r11;
	u64 r12;
	u64 r13;
	u64 r14;
	u64 r15;
	u128 xmm0;
	u128 xmm1;
	u128 xmm2;
	u128 xmm3;
	u128 xmm4;
	u128 xmm5;
} context_t, *pcontext_t;

#if WINVER > 1803
using vmexit_handler_t = void (__fastcall*)(pcontext_t* context, void* unknown);
#else
using vmexit_handler_t = void(__fastcall*)(pcontext_t context, void* unknown);
#endif

#pragma pack(push, 1)
typedef struct _voyager_t
{
	// RVA from golden record entry ---> back to original vmexit handler...
	u64 vcpu_run_rva;
	u64 hyperv_module_base;
	u64 hyperv_module_size;
	u64 record_base;
	u64 record_size;
} voyager_t, *pvoyager_t;
#pragma pack(pop)

__declspec(dllexport) inline voyager_t voyager_context;