#include "pg_table.h"

namespace pg_table
{
	void* translate(void* virtual_address, const ptable_entries entries)
	{
		virt_addr_t virt_addr{ virtual_address };
		virt_addr_t cursor{ hyperv_pml4 };

		if (entries) entries->pml4e = reinterpret_cast<ppml4e>(cursor.value)[virt_addr.pml4_index];
		if (!reinterpret_cast<ppml4e>(cursor.value)[virt_addr.pml4_index].present)
			return nullptr;

		// set the cursor to self reference so that when we read
		// the addresses pointed to by cursor its going to be a pdpt...
		cursor.pdpt_index = virt_addr_t{ hyperv_pml4 }.pml4_index;
		cursor.pd_index = virt_addr_t{ hyperv_pml4 }.pml4_index;
		cursor.pt_index = virt_addr.pml4_index;
		if (entries) entries->pdpte = reinterpret_cast<ppdpte>(cursor.value)[virt_addr.pdpt_index];

		if (!reinterpret_cast<ppdpte>(cursor.value)[virt_addr.pdpt_index].present)
			return nullptr;

		// set the cursor to self reference so that when we read
		// the addresses pointed to by cursor its going to be a pd...
		cursor.pdpt_index = virt_addr_t{ hyperv_pml4 }.pml4_index;
		cursor.pd_index = virt_addr.pml4_index;
		cursor.pt_index = virt_addr.pdpt_index;
		if (entries) entries->pde = reinterpret_cast<ppde>(cursor.value)[virt_addr.pd_index];

		if (!reinterpret_cast<ppde>(cursor.value)[virt_addr.pd_index].present)
			return nullptr;

		// set the cursor to self reference so that when we read
		// the addresses pointed to by cursor its going to be a pt...
		cursor.pdpt_index = virt_addr.pml4_index;
		cursor.pd_index = virt_addr.pdpt_index;
		cursor.pt_index = virt_addr.pd_index;
		if (entries) entries->pte = reinterpret_cast<ppte>(cursor.value)[virt_addr.pt_index];

		if (!reinterpret_cast<ppte>(cursor.value)[virt_addr.pt_index].present)
			return nullptr;

		return reinterpret_cast<void*>(
			reinterpret_cast<ppte>(cursor.value)[virt_addr.pt_index].pfn << 12);
	}

	void* translate(void* virtual_address, u32 pml4_pfn, const ptable_entries entries)
	{
		virt_addr_t virt_addr{ virtual_address };
		const auto cursor = get_cursor_page();

		set_cursor_page(pml4_pfn);
		if (!reinterpret_cast<ppml4e>(cursor)[virt_addr.pml4_index].present)
			return nullptr;

		if (entries) entries->pml4e = reinterpret_cast<ppml4e>(cursor)[virt_addr.pml4_index];
		set_cursor_page(reinterpret_cast<ppml4e>(cursor)[virt_addr.pml4_index].pfn);
		if (!reinterpret_cast<ppdpte>(cursor)[virt_addr.pdpt_index].present)
			return nullptr;

		if (entries) entries->pdpte = reinterpret_cast<ppdpte>(cursor)[virt_addr.pdpt_index];
		set_cursor_page(reinterpret_cast<ppdpte>(cursor)[virt_addr.pdpt_index].pfn);
		if (!reinterpret_cast<ppde>(cursor)[virt_addr.pd_index].present)
			return nullptr;

		if (entries) entries->pde = reinterpret_cast<ppde>(cursor)[virt_addr.pd_index];
		set_cursor_page(reinterpret_cast<ppde>(cursor)[virt_addr.pd_index].pfn);
		if (!reinterpret_cast<ppte>(cursor)[virt_addr.pt_index].present)
			return nullptr;

		if (entries) entries->pte = reinterpret_cast<ppte>(cursor)[virt_addr.pt_index];
		return reinterpret_cast<void*>(
			reinterpret_cast<ppte>(cursor)[virt_addr.pt_index].pfn << 12);
	}

	void set_cursor_page(u32 phys_pfn)
	{
		cpuid_eax_01 cpuid_value;
		__cpuid((int*)&cpuid_value, 1);
		pg_table::pt[cpuid_value
			.cpuid_additional_information
			.initial_apic_id].pfn = phys_pfn;

		// flush tlb for this page and then ensure the instruction stream
		// is seralized as to not execute instructions out of order and access the page
		// before the TLB is flushed...
		__invlpg(get_cursor_page());
		_mm_lfence();
	}

	void* get_cursor_page()
	{
		cpuid_eax_01 cpuid_value;
		__cpuid((int*)&cpuid_value, 1);
		constexpr auto cursor_page = 0x00007F7FFFE00000;

		virt_addr_t virt_addr{ reinterpret_cast<void*>(cursor_page) };
		virt_addr.pt_index = cpuid_value
			.cpuid_additional_information
			.initial_apic_id;

		return virt_addr.value;
	}

	bool init_pg_tables()
	{
		auto pdpt_phys = reinterpret_cast<u64>(translate(pdpt));
		auto pd_phys = reinterpret_cast<u64>(translate(pd));
		auto pt_phys = reinterpret_cast<u64>(translate(pt));

		if (!pdpt_phys || !pd_phys || !pt_phys)
			return false;

		hyperv_pml4[254].present = true;
		hyperv_pml4[254].pfn = pdpt_phys >> 12;
		hyperv_pml4[254].user_supervisor = false;
		hyperv_pml4[254].rw = true;

		pdpt[511].present = true;
		pdpt[511].pfn = pd_phys >> 12;
		pdpt[511].user_supervisor = false;
		pdpt[511].rw = true;

		pd[511].present = true;
		pd[511].pfn = pt_phys >> 12;
		pd[511].user_supervisor = false;
		pd[511].rw = true;

		for (auto idx = 0u; idx < 512; ++idx)
		{
			pt[idx].present = true;
			pt[idx].user_supervisor = false;
			pt[idx].rw = true;
		}
		return true;
	}
}