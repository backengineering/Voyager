#include "types.h"

void vmexit_handler(pcontext_t* context, void* unknown)
{
	DBG_PRINT("vmexit called....\n");

	// when hyper-v gets remapped out of winload's context
	// the linear virtual addresses change... thus an adjustment is required...
	reinterpret_cast<vmexit_handler_t>(
		reinterpret_cast<uintptr_t>(&vmexit_handler) - 
			voyager_context.vmexit_handler_rva)(context, unknown);
}