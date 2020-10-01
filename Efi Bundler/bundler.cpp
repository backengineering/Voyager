#include "bundler.h"

namespace bundler
{
	std::pair<std::u32_t, std::u32_t> add_section(std::vector<std::u8_t>& image, const char* name, std::size_t size, std::u32_t protect)
	{
		auto align = [](std::u32_t size, std::u32_t align, std::u32_t addr) -> std::u32_t
		{
			if (!(size % align))
				return addr + size;
			return addr + (size / align + 1) * align;
		};

		auto section_header = reinterpret_cast<PIMAGE_SECTION_HEADER>(
			((u64)&NT_HEADER(image.data())->OptionalHeader) +
				NT_HEADER(image.data())->FileHeader.SizeOfOptionalHeader);

		auto new_section = &section_header[NT_HEADER(image.data())->FileHeader.NumberOfSections];
		memset(new_section, NULL, sizeof(IMAGE_SECTION_HEADER));
		memcpy(new_section->Name, name, 8);

		new_section->Characteristics = protect;
		section_header[NT_HEADER(image.data())->FileHeader.NumberOfSections].Misc.VirtualSize =
			align(size, NT_HEADER(image.data())->OptionalHeader.SectionAlignment, NULL);

		new_section->VirtualAddress = align(section_header[
			NT_HEADER(image.data())->FileHeader.NumberOfSections - 1].Misc.VirtualSize,
				NT_HEADER(image.data())->OptionalHeader.SectionAlignment, section_header[
					NT_HEADER(image.data())->FileHeader.NumberOfSections - 1].VirtualAddress);

		new_section->SizeOfRawData =
			align(size, NT_HEADER(image.data())->OptionalHeader.FileAlignment, 0);

		new_section->PointerToRawData =
			align(section_header[NT_HEADER(image.data())->FileHeader.NumberOfSections - 1].SizeOfRawData,
				NT_HEADER(image.data())->OptionalHeader.FileAlignment, section_header[NT_HEADER(image.data())->
					FileHeader.NumberOfSections - 1].PointerToRawData);

		NT_HEADER(image.data())->OptionalHeader.SizeOfImage = section_header[
			NT_HEADER(image.data())->FileHeader.NumberOfSections].VirtualAddress +
				section_header[NT_HEADER(image.data())->FileHeader.NumberOfSections].Misc.VirtualSize;

		++NT_HEADER(image.data())->FileHeader.NumberOfSections;
		auto raw_data_rva = new_section->PointerToRawData;
		auto virt_data_rva = new_section->VirtualAddress;
		auto raw_data_size = new_section->SizeOfRawData;

		image.resize(raw_data_rva + raw_data_size);
		memset(image.data() + raw_data_rva, NULL, raw_data_size);
		return { raw_data_rva, virt_data_rva };
	}

	// module_base is .efi section base in this case...
	std::u32_t map_module(std::u8_t* module_base, std::vector<std::u8_t>& map_from)
	{
		// copy nt headers...
		memcpy(module_base, map_from.data(), NT_HEADER(map_from.data())->OptionalHeader.SizeOfHeaders);
		auto sections = reinterpret_cast<PIMAGE_SECTION_HEADER>(
			(u8*)&NT_HEADER(map_from.data())->OptionalHeader +
				NT_HEADER(map_from.data())->FileHeader.SizeOfOptionalHeader);

		// copy sections...
		for (auto i = 0u; i < NT_HEADER(map_from.data())->FileHeader.NumberOfSections; ++i)
		{
			auto section = &sections[i];
			memcpy(module_base + section->VirtualAddress, map_from.data() + section->PointerToRawData, section->SizeOfRawData);
		}

		return NT_HEADER(map_from.data())->OptionalHeader.AddressOfEntryPoint;
	}

	void bundle(std::vector<std::u8_t>& bundle_into, std::vector<std::u8_t>& bundle_module)
	{
		auto [trp_section_disk, trp_section_virt] = add_section(bundle_into, ".trp", sizeof shellcode::stub, SECTION_RWX);
		auto [mod_section_disk, mod_section_virt] = add_section(bundle_into, ".efi", bundle_module.size(), SECTION_RWX);
		bundler::map_module(bundle_into.data() + mod_section_disk, bundle_module);

		std::printf("[+] added .trp section at rva -> 0x%x, size -> 0x%x\n", trp_section_virt, sizeof shellcode::stub);
		std::printf("[+] added .efi section at rva -> 0x%x, size -> 0x%x\n", mod_section_virt, bundle_module.size());

		// setup stub shellcode...
		*reinterpret_cast<std::int32_t*>(&shellcode::stub[25]) = mod_section_virt - trp_section_virt;
		*reinterpret_cast<std::int32_t*>(&shellcode::stub[45]) = trp_section_virt;
		*reinterpret_cast<std::int32_t*>(&shellcode::stub[75]) = NT_HEADER(bundle_into.data())->OptionalHeader.AddressOfEntryPoint;
		memcpy(bundle_into.data() + trp_section_disk, shellcode::stub, sizeof shellcode::stub);
		std::printf("[+] added stub code to .trp section...\n");

		// set entry point to .trp section...
		NT_HEADER(bundle_into.data())->OptionalHeader.AddressOfEntryPoint = trp_section_virt;
		std::printf("[+] changed base modules entry point to -> (.trp section base) 0x%x\n", trp_section_virt);
	}
}