ENTRY(_start)

SECTIONS
{
.text.boot : {
    *(.text.boot)
}
.vectors : ALIGN(0x800) {
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
. = ALIGN(0x10);
.stack2 (NOLOAD) : {
    __stack2_start = .;
    . += 0x1000;
    __stack2_end = .;
}
. = ALIGN(0x10);
.stack3 (NOLOAD) : {
    __stack3_start = .;
    . += 0x1000;
    __stack3_end = .;
}
. = ALIGN(0x10);
.stack4 (NOLOAD) : {
    __stack4_start = .;
    . += 0x1000;
    __stack4_end = .;
}
}
