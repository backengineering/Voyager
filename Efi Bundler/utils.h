#pragma once
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <filesystem>
#include <string_view>
#include <iterator>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <ntstatus.h>
#include <winternl.h>
#include <array>
#include <algorithm>
#include <string_view>

#define NT_HEADER(x) reinterpret_cast<PIMAGE_NT_HEADERS>( std::uint64_t(x) + reinterpret_cast<PIMAGE_DOS_HEADER>(x)->e_lfanew )
namespace impl
{
	using uq_handle = std::unique_ptr<void, decltype(&CloseHandle)>;

	__forceinline std::uint32_t get_process_id(const std::wstring_view process_name)
	{
		// open a system snapshot of all loaded processes
		uq_handle snap_shot{ CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0), &CloseHandle };

		if (snap_shot.get() == INVALID_HANDLE_VALUE)
		{
			return 0;
		}

		PROCESSENTRY32W process_entry{ sizeof(PROCESSENTRY32W) };

		// enumerate through processes
		for (Process32FirstW(snap_shot.get(), &process_entry); Process32NextW(snap_shot.get(), &process_entry); )
			if (std::wcscmp(process_name.data(), process_entry.szExeFile) == 0)
				return process_entry.th32ProcessID;

		return 0;
	}

	__forceinline void open_binary_file(const std::string& file, std::vector<std::uint8_t>& data)
	{
		std::ifstream fstr(file, std::ios::binary);
		fstr.unsetf(std::ios::skipws);
		fstr.seekg(0, std::ios::end);

		const auto file_size = fstr.tellg();

		fstr.seekg(NULL, std::ios::beg);
		data.reserve(static_cast<std::uint32_t>(file_size));
		data.insert(data.begin(), std::istream_iterator<std::uint8_t>(fstr), std::istream_iterator<std::uint8_t>());
	}

	__forceinline bool enable_privilege(const std::wstring_view privilege_name)
	{
		HANDLE token_handle = nullptr;
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token_handle))
		{;
			return false;
		}

		LUID luid{};
		if (!LookupPrivilegeValueW(nullptr, privilege_name.data(), &luid))
		{
			return false;
		}

		TOKEN_PRIVILEGES token_state{};
		token_state.PrivilegeCount = 1;
		token_state.Privileges[0].Luid = luid;
		token_state.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (!AdjustTokenPrivileges(token_handle, FALSE, &token_state, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr))
		{
			return false;
		}

		CloseHandle(token_handle);

		return true;
	}
}