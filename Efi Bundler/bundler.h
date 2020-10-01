#pragma once
#include "shellcode.h"
#define SECTION_RWX ((IMAGE_SCN_MEM_WRITE | \
	IMAGE_SCN_CNT_CODE | \
	IMAGE_SCN_CNT_UNINITIALIZED_DATA | \
	IMAGE_SCN_MEM_EXECUTE | \
	IMAGE_SCN_CNT_INITIALIZED_DATA | \
	IMAGE_SCN_MEM_READ))

namespace bundler
{
	std::pair<std::uint32_t, std::uint32_t> add_section(std::vector<std::uint8_t>& image, const char* name, std::size_t size, std::uint32_t protect);
	std::uint32_t map_module(std::uint8_t* module_base, std::vector<std::uint8_t>& map_from);
	void bundle(std::vector<std::uint8_t>& bundle_into, std::vector<std::uint8_t>& bundle_module);
}
