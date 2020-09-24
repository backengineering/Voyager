#include <iostream>
#include "com.h"

int main()
{
	std::printf("[+] hyper-v (CPUID) result -> 0x%x\n", cpuid_test(VMEXIT_KEY));
	std::getchar();
}