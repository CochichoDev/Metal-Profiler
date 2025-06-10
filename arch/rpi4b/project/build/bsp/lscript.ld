/* Translation Table Linker Script */

MEMORY
{
	MEM_REGION_0_SHARED :	 ORIGIN = 0x10000, LENGTH = 0x12000
}

ENTRY(MMUTableL0.0)

SECTIONS
{
/DISCARD/ : {
    *(.text)
    *(.data)
    *(.bss)
    *(.comment)
    *(.note*)
}
.mmu_tbl0 : {
	. = ALIGN(4096);
	__mmu_tbl0_start = .;
	*(.mmu_tbl0)
	__mmu_tbl0_end = .;
} > MEM_REGION_0_SHARED
.mmu_tbl1 : {
	. = ALIGN(4096);
	__mmu_tbl1_start = .;
	*(.mmu_tbl1)
	__mmu_tbl1_end = .;
} > MEM_REGION_0_SHARED
.mmu_tbl2 : {
	. = ALIGN(4096);
	__mmu_tbl2_start = .;
	*(.mmu_tbl2)
	__mmu_tbl2_end = .;
} > MEM_REGION_0_SHARED
.mmu_tbl3 : {
	. = ALIGN(4096);
	__mmu_tbl3_start = .;
	*(.mmu_tbl3)
	__mmu_tbl3_end = .;
} > MEM_REGION_0_SHARED
}
