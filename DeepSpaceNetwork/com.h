#pragma once
#define VMEXIT_KEY 0xDEADBEEFDEADBEEF
extern "C" size_t cpuid_test(size_t key);