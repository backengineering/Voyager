#include <iostream>
#include "com.h"

int main()
{
	std::printf("[+] hyper-v (CPUID) result -> 0x%x\n", hyperv(VMEXIT_KEY));
	std::getchar();
}