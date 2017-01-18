/*      AT91SAM3S4C MCU Linker script  */

OUTPUT_FORMAT("elf32-littlearm")

MEMORY
{
  BOOT (rx):       ORIGIN = 0x00000000, LENGTH = 4M
  VECT (rx):       ORIGIN = 0x00400000, LENGTH = 4k 
  FLASH (rx):      ORIGIN = 0x0043F000, LENGTH = 4k
  ROM (rx) :       ORIGIN = 0x00800000, LENGTH = 4M
  RAM (rw):        ORIGIN = 0x20000000, LENGTH = 48k
}

ENTRY(__vector_table)

/* Size of single Main Stack used by the application. Process Stack is not used. */
MAIN_STACK_SIZE = 0x1000;        /* 4 kB         */
PROCESS_STACK_SIZE = 0;

SECTIONS
{
   /* Cortex-M3 vectors table with exceptions and reset handlers. Should be placed at the beginning of flash. */
   .vectors :
   {
         __vectors = .;
         KEEP(*(.vectors))
         . = ALIGN(4);
         __vectors_end = .;
   } > VECT

  /* Code */
   .text :
  {
        __text = .;
        *(.text)
        *(.text.*)
        *(.gnu.linkonce.t.*)
        *(.rodata)              /* read-only data (constants) */
        *(.rodata.*)
        *(.gnu.linkonce.r.*)
        . = ALIGN(4);
        __text_end = .;
  } > FLASH

  /* Non-zero initialized data */
  .data :
  {
      __data_load = LOADADDR(.data);
      __data = .;
      *(.data)
      *(.data.*)
      *(.gnu.linkonce.d.*)
      . = ALIGN(4);
      __data_end = .;
  } >RAM AT>FLASH

  /* Zero-initialized data */
  .bss (NOLOAD) :
  {
      __bss = .;
      *(.bss)
      *(.bss.*)
      *(.gnu.linkonce.b.*)
      *(COMMON)
      . = ALIGN(4);
      __bss_end = .;
  } >RAM

  /* Stacks. The EABI requires the stack be 8-byte aligned. */
  .stack ALIGN(8):
  {
      __stack_bottom = .;
      __process_stack_bottom = .;

      . += MAIN_STACK_SIZE;
      . = ALIGN(8);

      __process_stack_top = .;
      __main_stack_bottom = .;

      . += PROCESS_STACK_SIZE;
      . = ALIGN(8);

      __main_stack_top = .;
      __stack_top = .;
  } >RAM

  __end = .;

  /* Stabs debugging sections.  */
  .stab          0 : { *(.stab) }
  .stabstr       0 : { *(.stabstr) }
  .stab.excl     0 : { *(.stab.excl) }
  .stab.exclstr  0 : { *(.stab.exclstr) }
  .stab.index    0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment       0 : { *(.comment) }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
}
