#include "types.h"
#include "ia32.hpp"
#define VMEXIT_KEY 0xDEADBEEFDEADBEEF

#if WINVER > 1803
void vmexit_handler(pcontext_t* context, void* unknown)
#else
void vmexit_handler(pcontext_t context, void* unknown)
#endif
{
#if WINVER > 1803
	pcontext_t guest_registers = *context;
#else
	pcontext_t guest_registers = context;
#endif

	size_t vmexit_reason;
	__vmx_vmread(VMCS_EXIT_REASON, &vmexit_reason);
	if (vmexit_reason == VMX_EXIT_REASON_EXECUTE_CPUID)
	{
		if (guest_registers->rcx == VMEXIT_KEY)
		{
			guest_registers->rax = 0xC0FFEE;

			// advance rip, no one better execute cpuid instruction
			// with 0xDEADBEEFDEADBEEF in RCX...
			size_t rip, exec_len;
			__vmx_vmread(VMCS_GUEST_RIP, &rip);
			__vmx_vmread(VMCS_VMEXIT_INSTRUCTION_LENGTH, &exec_len);
			__vmx_vmwrite(VMCS_GUEST_RIP, rip + exec_len);
			return;
		}
	}

	// when hyper-v gets remapped out of winload's context
	// the linear virtual addresses change... thus an adjustment is required...
	reinterpret_cast<vmexit_handler_t>(
		reinterpret_cast<uintptr_t>(&vmexit_handler) - 
			voyager_context.vmexit_handler_rva)(context, unknown);
}