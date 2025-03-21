ENTRY(_boot)

_BUFFER_SIZE = 0x200000;

MEMORY
{
    ram (rwx) : ORIGIN = 0x000010000, LENGTH = 0x100000
    ram2 (rwx) : ORIGIN = 0x000100000, LENGTH = 0x200000
}

SECTIONS
{
.mmu0 : ALIGN(0x1000) {
    *(.mmu_tbl0)
} > ram
.mmu1 : ALIGN(0x1000) {
    *(.mmu_tbl1)
} > ram
.mmu2 : ALIGN(0x1000) {
    *(.mmu_tbl2)
} > ram

.boot : ALIGN(0x8) {
    *(.boot)
} > ram
.text : {
    *(.text)
    *(text.*)
} > ram
.bss (NOLOAD) : ALIGN(0x8) {
    __bss_start = .;
    *(.bss)
    *(.bss.*)
    . = ALIGN(0x8);
    __bss_end = .;
} > ram
.data : ALIGN(0x8) {
    *(.data)
    *(.data.*)
} > ram
.rodata : ALIGN(0x8) {
    *(.rodata)
    *(.rodata.*)
} > ram
.stack (NOLOAD) : ALIGN(0x10) {
    __stack_start = .;
    . += 0x1000;
    __stack_end = .;
} > ram
.buffer : {
	. = ALIGN(64);
	__buffer_start = .;
	. += _BUFFER_SIZE;
	__buffer_end = .;
} > ram2
}
