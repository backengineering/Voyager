#include "types.h"

svm::pgs_base_struct vmexit_handler(void* unknown, svm::pguest_context context)
{
	const auto vmcb = *reinterpret_cast<svm::pvmcb*>(
		*reinterpret_cast<u64*>(
			*reinterpret_cast<u64*>(
				__readgsqword(0) + offset_vmcb_base) 
					+ offset_vmcb_link) + offset_vmcb);

	if (vmcb->exitcode == VMEXIT_CPUID && context->rcx == VMEXIT_KEY)
	{
		vmcb->rax = 0xC0FFEE;
		vmcb->rip = vmcb->nrip;
		return reinterpret_cast<svm::pgs_base_struct>(__readgsqword(0));
	}

	return reinterpret_cast<svm::vcpu_run_t>(
		reinterpret_cast<u64>(&vmexit_handler) -
			svm::voyager_context.vcpu_run_rva)(unknown, context);
}