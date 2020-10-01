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
	std::pair<std::u32_t, std::u32_t> add_section(std::vector<std::u8_t>& image, const char* name, std::size_t size, std::u32_t protect);
	std::u32_t map_module(std::u8_t* module_base, std::vector<std::u8_t>& map_from);
	void bundle(std::vector<std::u8_t>& bundle_into, std::vector<std::u8_t>& bundle_module);
}
