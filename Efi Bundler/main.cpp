#include "bundler.h"

int __cdecl main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::printf("[!] invalid amount of parameters\n");
		return -1;
	}

	std::vector<std::u8_t> efi_module;
	std::vector<std::u8_t> bootmgfw;

	impl::open_binary_file(argv[1], bootmgfw);
	impl::open_binary_file(argv[2], efi_module);

	if (efi_module.empty() || bootmgfw.empty())
	{
		std::printf("[!] unable to load efi module(s)...\n");
		return -1;
	}

	efi_module.resize(NT_HEADER(efi_module.data())->OptionalHeader.SizeOfImage);
	std::printf("bundling efi module, size -> 0x%x\n",
		NT_HEADER(efi_module.data())->OptionalHeader.SizeOfImage);

	bootmgfw.resize(NT_HEADER(bootmgfw.data())->OptionalHeader.SizeOfImage);
	std::printf("bundling module into bootmgfw, size before patch -> 0x%x\n", 
		NT_HEADER(bootmgfw.data())->OptionalHeader.SizeOfImage);

	bundler::bundle(bootmgfw, efi_module);
	std::ofstream new_file("result.efi", std::ios::binary);
	new_file.write((char*)bootmgfw.data(), bootmgfw.size());
	new_file.close();

	std::printf("bundled modules together....\n");
	std::getchar();
}