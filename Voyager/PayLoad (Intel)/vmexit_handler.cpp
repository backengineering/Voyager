#include "pg_table.h"

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
			switch ((vmexit_command_t)(guest_registers->rdx))
			{
			case vmexit_command_t::init_paging_tables:
				guest_registers->rax = pg_table::init_pg_tables();
				break;
			default:
				break;
			}

			size_t rip, exec_len;
			__vmx_vmread(VMCS_GUEST_RIP, &rip);
			__vmx_vmread(VMCS_VMEXIT_INSTRUCTION_LENGTH, &exec_len);
			__vmx_vmwrite(VMCS_GUEST_RIP, rip + exec_len);
			return;
		}
	}

	// since there are alot of contexts being created and switched about,
	// all hooks are done relative inside of hyper-v...
	reinterpret_cast<vmexit_handler_t>(
		reinterpret_cast<u64>(&vmexit_handler) -
			voyager_context.vmexit_handler_rva)(context, unknown);
}