#pragma once
#define VMEXIT_KEY 0xDEADBEEFDEADBEEF
enum class vmexit_command_t
{
	init_paging_tables = 0x111
	// add your commands here...
};

extern "C" size_t hypercall(size_t key, vmexit_command_t command);