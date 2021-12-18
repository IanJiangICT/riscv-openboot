# riscv-openboot
Bootloader for RISC-V processors.

## Memory Space

```
                 <- low                                          high->

                   Segment 0      1          2        ...      x
ROM on-chip      |----------|----------|----------| ~ ~ ~ ~|----------|
(or off-chip)     ZSBL-MC    BootConf0  SeltTest(optinal)   ZSBL-PC(optinal)
                 ^          ^          ^                   ^
    PLAT_ROM_BASE|          |          |                   |PLAT_ROM_OFFSET_PC
                 |          |          |PLAT_ROM_OFFSET_ST
                 |          |PLAT_ROM_OFFSET_BC0
                 |PLAT_ROM_OFFSET_MC


RAM on-chip      |--------------------------------- ~ ~ ~ ~-----------|
                  FSBL            BootConf      Stack-MC    Stacks-PC
                 ^               ^             ^           ^
    PLAT_RAM_BASE|               |             |           |PLAT_RAM_STACK_BASE_PC
                 |               |             |PLAT_RAM_STACK_BASE_MC
                 |               |PLAT_RAM_BC
                 |PLAT_RAM_FSBL

DDR              |--------------------------------- ~ ~ ~ ~-----------|
(RAM off-chip)    Reserved   ShareZone   OpenSBI
                 ^          ^           ^
    PLAT_DDR_BASE|          |           |PLAT_DDR_OPENSBI
                            |PLAT_DDR_SHAREZONE
```
