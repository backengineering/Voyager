#pragma once
#define VMEXIT_KEY 0xDEADBEEFDEADBEEF
extern "C" size_t hyperv(size_t key);