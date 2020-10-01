#include "types.h"

#define VMEXIT_KEY 0xDEADBEEFDEADBEEF
svm::pgs_base_struct vcpu_run(svm::pguest_context context)
{
	while (1)
	{
		__svm_clgi();
		svm::pgs_base_struct result = reinterpret_cast<svm::vcpu_run_t>(
			reinterpret_cast<uintptr_t>(&vcpu_run) -
				svm::voyager_context.vcpu_run_rva)(context);
		__svm_stgi();

		// gs:0 + 0x103B0 ] + 0x198 ] + 0xE80 ] = pointer to vmcb...
		auto vmcb = *reinterpret_cast<svm::pvmcb*>(
			*reinterpret_cast<uintptr_t*>(
				reinterpret_cast<uintptr_t>(
					result->pvcpu_context) + 0x198) + 0xE80);

		if (vmcb->exitcode == VMEXIT_CPUID && context->rcx == VMEXIT_KEY)
		{
			vmcb->rax = 0xC0FFEE;
			vmcb->rip = vmcb->nrip;
			continue;
		}

		// else return result...
		return result;
	}
}