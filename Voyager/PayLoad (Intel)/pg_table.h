#pragma once
#include "types.h"
#include "ia32.hpp"

#pragma section(".pdpt", read, write)
#pragma section(".pd", read, write)
#pragma section(".pt", read, write)

namespace pg_table
{
    typedef union _virt_addr_t
    {
        void* value;
        struct
        {
            u64 offset : 12;
            u64 pt_index : 9;
            u64 pd_index : 9;
            u64 pdpt_index : 9;
            u64 pml4_index : 9;
            u64 reserved : 16;
        };
    } virt_addr_t, * pvirt_addr_t;

    typedef union _pml4e
    {
        u64 value;
        struct
        {
            u64 present : 1;          // Must be 1, region invalid if 0.
            u64 rw : 1;        // If 0, writes not allowed.
            u64 user_supervisor : 1;   // If 0, user-mode accesses not allowed.
            u64 PageWriteThrough : 1; // Determines the memory type used to access PDPT.
            u64 page_cache : 1; // Determines the memory type used to access PDPT.
            u64 accessed : 1;         // If 0, this entry has not been used for translation.
            u64 Ignored1 : 1;
            u64 page_size : 1;         // Must be 0 for PML4E.
            u64 Ignored2 : 4;
            u64 pfn : 36; // The page frame number of the PDPT of this PML4E.
            u64 Reserved : 4;
            u64 Ignored3 : 11;
            u64 nx : 1; // If 1, instruction fetches not allowed.
        };
    } pml4e, * ppml4e;

    typedef union _pdpte
    {
        u64 value;
        struct
        {
            u64 present : 1;          // Must be 1, region invalid if 0.
            u64 rw : 1;        // If 0, writes not allowed.
            u64 user_supervisor : 1;   // If 0, user-mode accesses not allowed.
            u64 PageWriteThrough : 1; // Determines the memory type used to access PD.
            u64 page_cache : 1; // Determines the memory type used to access PD.
            u64 accessed : 1;         // If 0, this entry has not been used for translation.
            u64 Ignored1 : 1;
            u64 page_size : 1;         // If 1, this entry maps a 1GB page.
            u64 Ignored2 : 4;
            u64 pfn : 36; // The page frame number of the PD of this PDPTE.
            u64 Reserved : 4;
            u64 Ignored3 : 11;
            u64 nx : 1; // If 1, instruction fetches not allowed.
        };
    } pdpte, * ppdpte;

    typedef union _pde
    {
        u64 value;
        struct
        {
            u64 present : 1;          // Must be 1, region invalid if 0.
            u64 rw : 1;        // If 0, writes not allowed.
            u64 user_supervisor : 1;   // If 0, user-mode accesses not allowed.
            u64 PageWriteThrough : 1; // Determines the memory type used to access PT.
            u64 page_cache : 1; // Determines the memory type used to access PT.
            u64 accessed : 1;         // If 0, this entry has not been used for translation.
            u64 Ignored1 : 1;
            u64 page_size : 1; // If 1, this entry maps a 2MB page.
            u64 Ignored2 : 4;
            u64 pfn : 36; // The page frame number of the PT of this PDE.
            u64 Reserved : 4;
            u64 Ignored3 : 11;
            u64 nx : 1; // If 1, instruction fetches not allowed.
        };
    } pde, * ppde;

    typedef union _pte
    {
        u64 value;
        struct
        {
            u64 present : 1;          // Must be 1, region invalid if 0.
            u64 rw : 1;        // If 0, writes not allowed.
            u64 user_supervisor : 1;   // If 0, user-mode accesses not allowed.
            u64 PageWriteThrough : 1; // Determines the memory type used to access the memory.
            u64 page_cache : 1; // Determines the memory type used to access the memory.
            u64 accessed : 1;         // If 0, this entry has not been used for translation.
            u64 Dirty : 1;            // If 0, the memory backing this page has not been written to.
            u64 PageAccessType : 1;   // Determines the memory type used to access the memory.
            u64 Global : 1;           // If 1 and the PGE bit of CR4 is set, translations are global.
            u64 Ignored2 : 3;
            u64 pfn : 36; // The page frame number of the backing physical page.
            u64 reserved : 4;
            u64 Ignored3 : 7;
            u64 ProtectionKey : 4;  // If the PKE bit of CR4 is set, determines the protection key.
            u64 nx : 1; // If 1, instruction fetches not allowed.
        };
    } pte, * ppte;

    typedef struct _table_entries
    {
        pg_table::pml4e pml4e;
        pg_table::pdpte pdpte;
        pg_table::pde   pde;
        pg_table::pte   pte;
    } table_entries, *ptable_entries;

    /// <summary>
    /// payload paging tables...
    /// </summary>
    __declspec(allocate(".pdpt")) inline pdpte pdpt[512];
    __declspec(allocate(".pd")) inline pde pd[512];
    __declspec(allocate(".pt")) inline pte pt[512];

    /// <summary>
    /// self referencing pml4e is at 255...
    /// </summary>
    inline const ppml4e hyperv_pml4{ reinterpret_cast<ppml4e>(0x00007FBFDFEFF000) };    
    
    /// <summary>
    /// only does address translation for hyper-v's context
    /// </summary>
    /// <param name="virtual_address">virtual address to be translated...</param>
    /// <param name="entries">optional </param>
    /// <returns>returns a physical address...</returns>
    void* translate(void* virt_addr, const ptable_entries entries = nullptr);

    /// <summary>
    /// translates linear virtual addresses to linear physical addresses...
    /// </summary>
    /// <param name="virtual_address">virtual address to translate...</param>
    /// <param name="pml4_pfn">page map level four page frame number...</param>
    /// <param name="entries">(optional) pointer to a table_entries structure...</param>
    /// <returns>linear physical address...</returns>
    void* translate(void* virtual_address, u32 pml4_pfn, const ptable_entries entries = nullptr);

    /// <summary>
    //  changes the cursor address to the specified physical address...
    //  after doing so, the TLB entry for that address is going to be flushed...
    //  a memory fence is applied to prevent out of order execution...
    /// </summary>
    /// <param name="phys_pfn">pfn of the physical page to change the cursor too...</param>
    void set_cursor_page(u32 phys_pfn);

    /// <summary>
    /// get the cursor page... each core has its own cursor page...
    /// </summary>
    /// <returns>cursor page for the current core...</returns>
    void* get_cursor_page();

    /// <summary>
    /// initalizes paging tables (connects pdpt->pd->pt)
    /// </summary>
    /// <returns>was the setup successful?</returns>
    bool init_pg_tables();
}