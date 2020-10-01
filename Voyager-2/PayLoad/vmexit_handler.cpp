#include "types.h"

svm::pgs_base_struct vmexit_handler(void* unknown, svm::pguest_context context)
{
	// gs:0 + 0x103B0 ] + 0x198 ] + 0xE80 ] = pointer to vmcb...
	const auto vmcb = *reinterpret_cast<svm::pvmcb*>(
		*reinterpret_cast<uintptr_t*>(
			*reinterpret_cast<uintptr_t*>(
				__readgsqword(0) + 0x103B0) + 0x198) + 0xE80);

	if (vmcb->exitcode == VMEXIT_CPUID && context->rcx == VMEXIT_KEY)
	{
		vmcb->rax = 0xC0FFEE;
		vmcb->rip = vmcb->nrip;
		return reinterpret_cast<svm::pgs_base_struct>(__readgsqword(0));
	}

	return reinterpret_cast<svm::vcpu_run_t>(
		reinterpret_cast<uintptr_t>(&vmexit_handler) -
			svm::voyager_context.vcpu_run_rva)(unknown, context);
}