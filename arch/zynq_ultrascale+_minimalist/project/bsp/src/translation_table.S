/*
 *Automatically generated by AutoMetalBench
 *Please make sure to define the attributes especified for each descriptor
 */

.set NEXT_PAGE_ATTR,    0x3
.set RESERVED,          0x0 					
.set MEMORY,	        0x405 | (3 << 8) | (0x0)
.set DEVICE,	        0x409 | (1 << 53)| (1 << 54) |(0x0)
.set MEM2,	            0x407 | (3 << 8) | (0x0)

.global MMUTableL0.0

.section .mmu_tbl0, "a"

MMUTableL0.0:
.set SECT, MMUTableL1.0
.rept 0x2
.8byte SECT + NEXT_PAGE_ATTR
.set SECT, SECT + 0x1000
.endr

.section .mmu_tbl1, "a"

MMUTableL1.0:
.set SECT, MMUTableL2.0
.rept 0x4
.8byte SECT + NEXT_PAGE_ATTR
.set SECT, SECT + 0x1000
.endr

MMUTableL1.1:
.set SECT, 0x100000000
.rept 0xc
.8byte SECT + RESERVED
.set SECT, SECT + 0x40000000
.endr

MMUTableL1.2:
.set SECT, 0x400000000
.rept 0x10
.8byte SECT + DEVICE
.set SECT, SECT + 0x40000000
.endr

MMUTableL1.3:
.set SECT, 0x800000000
.rept 0x2
.8byte SECT + MEMORY
.set SECT, SECT + 0x40000000
.endr

MMUTableL1.4:
.set SECT, 0x880000000
.rept 0x6
.8byte SECT + RESERVED
.set SECT, SECT + 0x40000000
.endr

MMUTableL1.5:
.set SECT, 0x1000000000
.rept 0x2c0
.8byte SECT + DEVICE
.set SECT, SECT + 0x40000000
.endr

MMUTableL1.6:
.set SECT, 0xc000000000
.rept 0x100
.8byte SECT + RESERVED
.set SECT, SECT + 0x40000000
.endr

.section .mmu_tbl2, "a"

MMUTableL2.0:
.set SECT, 0x0
.rept 0x3
.8byte SECT + MEMORY
.set SECT, SECT + 0x200000
.endr

MMUTableL2.1:
.set SECT, MMUTableL3.0
.rept 0x3fd
.8byte SECT + NEXT_PAGE_ATTR
.set SECT, SECT + 0x1000
.endr

MMUTableL2.2:
.set SECT, 0x80000000
.rept 0x380
.8byte SECT + DEVICE
.set SECT, SECT + 0x200000
.endr

MMUTableL2.3:
.set SECT, 0xf0000000
.rept 0x40
.8byte SECT + RESERVED
.set SECT, SECT + 0x200000
.endr

MMUTableL2.4:
.set SECT, 0xf8000000
.rept 0x9
.8byte SECT + DEVICE
.set SECT, SECT + 0x200000
.endr

MMUTableL2.5:
.set SECT, 0xf9200000
.rept 0x1f
.8byte SECT + RESERVED
.set SECT, SECT + 0x200000
.endr

MMUTableL2.6:
.set SECT, 0xfd000000
.rept 0x17
.8byte SECT + DEVICE
.set SECT, SECT + 0x200000
.endr

MMUTableL2.7:
.set SECT, 0xffe00000
.rept 0x1
.8byte SECT + MEMORY
.set SECT, SECT + 0x200000
.endr

.section .mmu_tbl3, "a"

MMUTableL3.0:
#ifdef CACHECOLORING

.set SECT, 0x600000 + 0x0
.rept 0x7fa0
.rept 0x4
.8byte SECT + MEM2
.set SECT, SECT + 0x1000
.endr
.set SECT, SECT + 0xc000
.endr

.set SECT, 0x600000 + 0x4000
.rept 0x7fa0
.rept 0x4
.8byte SECT + MEM2
.set SECT, SECT + 0x1000
.endr
.set SECT, SECT + 0xc000
.endr

.set SECT, 0x600000 + 0x8000
.rept 0x7fa0
.rept 0x4
.8byte SECT + MEM2
.set SECT, SECT + 0x1000
.endr
.set SECT, SECT + 0xc000
.endr

.set SECT, 0x600000 + 0xc000
.rept 0x7fa0
.rept 0x4
.8byte SECT + MEM2
.set SECT, SECT + 0x1000
.endr
.set SECT, SECT + 0xc000
.endr

#else
.set SECT, 0x600000
.rept 0x7fa00
.8byte SECT + MEM2
.set SECT, SECT + 0x1000
.endr

#endif

