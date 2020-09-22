#include "types.h"

void vmexit_handler(pcontext_t* context, void* unknown1, void* unknown2, void* unknown3)
{
	DBG_PRINT("vmexit called....\n");
	DBG_PRINT("calling original vmexit handler....\n");
	pvoyager_context->vmexit_handler(context, unknown1, unknown2, unknown3);
}