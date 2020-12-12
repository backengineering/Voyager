#include "types.h"
#include "pg_table.h"

svm::pgs_base_struct vmexit_handler(void* unknown, svm::pguest_context context)
{
	// AMD does not have a vmread/vmwrite instruction... only a vmload
	// and vmsave instruction... this means I had to hunt down the damn
	// VMCB location... this is the pointer chain to the VMCB...
	//
	// TODO: could sig scan for this in Voyager...
	const auto vmcb = *reinterpret_cast<svm::pvmcb*>(
		*reinterpret_cast<u64*>(
			*reinterpret_cast<u64*>(
				__readgsqword(0) + offset_vmcb_base) 
					+ offset_vmcb_link) + offset_vmcb);

	if (vmcb->exitcode == VMEXIT_CPUID && context->rcx == VMEXIT_KEY)
	{
		switch ((svm::vmexit_command_t)context->rdx)
		{
		case svm::vmexit_command_t::init_paging_tables:
			vmcb->rax = pg_table::init_pg_tables();
			break;
		default:
			break;
		}

		// advance RIP and return back to VMLOAD/VMRUN...
		vmcb->rip = vmcb->nrip;
		return reinterpret_cast<svm::pgs_base_struct>(__readgsqword(0));
	}

	return reinterpret_cast<svm::vcpu_run_t>(
		reinterpret_cast<u64>(&vmexit_handler) -
			svm::voyager_context.vcpu_run_rva)(unknown, context);
}