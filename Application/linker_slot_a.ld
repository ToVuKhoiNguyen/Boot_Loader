/***************************************************************************//**
 * GCC Linker script for Application — Slot A (0x00010200)
 *******************************************************************************
 * Based on Silicon Labs autogen linkerfile.ld template.
 * FLASH ORIGIN set to 0x00010200 (after 512-byte header block).
 * LENGTH = 0x0000FE00 (65024 bytes = 64KB slot - 512B header).
 ******************************************************************************/

 MEMORY
 {
   FLASH      (rx)  : ORIGIN = 0x00010200, LENGTH = 0x0000FE00
   RAM        (rwx) : ORIGIN = 0x20000000, LENGTH = 0x8000
 }

ENTRY(Reset_Handler)

SECTIONS
{

  .vectors :
  {
    linker_vectors_begin = .;
    KEEP(*(.vectors))
    linker_vectors_end = .;

    __Vectors_End = .;
    __Vectors_Size = __Vectors_End - __Vectors;
  } > FLASH

  .stack (NOLOAD):
  {
    . = ALIGN(8);
    __StackLimit = .;
    KEEP(*(.stack*))
    . = ALIGN(4);
    __StackTop = .;
    PROVIDE(__stack = __StackTop);
  } > RAM

  .bss :
  {
    . = ALIGN(4);
    __bss_start__ = .;
    *(SORT_BY_ALIGNMENT(.bss*))
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
  } > RAM


  .noinit (NOLOAD) : ALIGN(4)
  {
    *(.noinit*);
  } > RAM


  text_application_ram : ALIGN(4)
  {
    . = ALIGN(4);
    __vma_ramfuncs_start__ = .;
    __text_application_ram_start__ = .;

    *(text_application_ram)

    . = ALIGN(4);
    __vma_ramfuncs_end__ = .;
    __text_application_ram_end__ = .;
  } > RAM AT > FLASH

  __lma_ramfuncs_start__ = LOADADDR(text_application_ram);
  __lma_ramfuncs_end__ = LOADADDR(text_application_ram) + SIZEOF(text_application_ram);

  .rodata (READONLY):
  {
    __rodata_end__ = .;
  } > FLASH

  .text :
  {
    linker_code_begin = .;
    *(SORT_BY_ALIGNMENT(.text*))
    *(SORT_BY_ALIGNMENT(text_*))
    . = ALIGN(32);
    linker_code_end = .;

    KEEP(*(.init))
    KEEP(*(.fini))

    /* .ctors */
    *crtbegin.o(.ctors)
    *crtbegin?.o(.ctors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
    *(SORT(.ctors.*))
    *(.ctors)

    /* .dtors */
    *crtbegin.o(.dtors)
    *crtbegin?.o(.dtors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
    *(SORT(.dtors.*))
    *(.dtors)

    *(.rodata*)
    *(.eh_frame*)
  } > FLASH

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > FLASH

  __exidx_start = .;
  .ARM.exidx :
  {
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  } > FLASH
  __exidx_end = .;

  .copy.table (READONLY):
  {
    . = ALIGN(4);
    __copy_table_start__ = .;

    LONG (__etext)
    LONG (__data_start__)
    LONG ((__data_end__ - __data_start__) / 4)

    __copy_table_end__ = .;
  } > FLASH

  .zero.table (READONLY):
  {
    . = ALIGN(4);
    __zero_table_start__ = .;

    __zero_table_end__ = .;
    __etext = .;
  } > FLASH

  .data :
  {
    . = ALIGN(4);
    __data_start__ = .;
    *(vtable)
    *(SORT_BY_ALIGNMENT(.data*))
    . = ALIGN(4);

    . = ALIGN(4);
    /* preinit data */
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP(*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);

    . = ALIGN(4);
    /* init data */
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array))
    PROVIDE_HIDDEN (__init_array_end = .);

    . = ALIGN(4);
    /* finit data */
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array))
    PROVIDE_HIDDEN (__fini_array_end = .);

    . = ALIGN(4);
    /* All data end */
    __data_end__ = .;

  } > RAM AT > FLASH


/* Calculate heap size based on RAM limits. */
heap_limit = ORIGIN(RAM) + LENGTH(RAM);  /* End of RAM */
heap_size = heap_limit - __HeapBase;
  .heap (NOLOAD):
  {
    . = ALIGN(8);
    __HeapBase = .;
    . += heap_size;
    __end__ = .;
    _end = __end__;
    KEEP(*(.heap*))
    __HeapLimit = ORIGIN(RAM) + LENGTH(RAM);
  } > RAM

  __heap_size = __HeapLimit - __HeapBase;
  __ram_end__ = 0x20000000 + 0x8000;
  __main_flash_end__ = 0x00010200 + 0x0000FE00;

  .internal_storage (DSECT) : {
    KEEP(*(.internal_storage*))
  } > FLASH

  .nvm (DSECT) : {
    KEEP(*(.simee*))
  } > FLASH

  __ramfuncs_start__ = __vma_ramfuncs_start__;
  __ramfuncs_end__ = __vma_ramfuncs_end__;

  linker_nvm_end = __main_flash_end__;
  linker_nvm_begin = linker_nvm_end - SIZEOF(.nvm);
  linker_storage_end = linker_nvm_begin;
  __nvm3Base = linker_nvm_begin;

  linker_storage_begin = linker_storage_end - SIZEOF(.internal_storage);
  ASSERT((linker_storage_begin >= (__etext + SIZEOF(.data))), "FLASH memory overflowed !")

}
