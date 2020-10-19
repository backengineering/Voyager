#include <iostream>
#include "com.h"

int main()
{
	auto result = hypercall(VMEXIT_KEY, vmexit_command_t::init_paging_tables);
	std::printf("[+] hyper-v (CPUID) init page table result -> %d\n", result);
	std::getchar();
}