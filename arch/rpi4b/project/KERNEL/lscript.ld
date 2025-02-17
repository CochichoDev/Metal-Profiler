ENTRY(_start)

SECTIONS
{
.text.boot : {
    *(.text.boot)
}
.vectors : ALIGN(11) {
    *(.vectors)
    *(.vectors.*)
}
.text : {
    *(.text)
    *(text.*)
}
. = ALIGN(0x8);
.bss (NOLOAD) : {
    __bss_start = .;
    *(.bss)
    *(bss.*)
    . = ALIGN(0x8);
    __bss_end = .;
}
. = ALIGN(0x8);
.data : {
    *(.data)
    *(.data.*)
}
. = ALIGN(0x8);
.rodata : {
    *(.rodata)
    *(.rodata.*)
}
. = ALIGN(0x10);
.stack (NOLOAD) : {
    __stack_start = .;
    . += 0x1000;
    __stack_end = .;
}
}
