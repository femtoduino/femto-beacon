OUTPUT_FORMAT("elf32-avr")
OUTPUT_ARCH(avr:5)

MEMORY
{
  text   (rx)   : ORIGIN = 0x0001F000, LENGTH = 4K
  access (rx)   : ORIGIN = 0x0001FFFE, LENGTH = 2
  data   (rw!x) : ORIGIN = 0x00800200, LENGTH = 8K - 500  /* leave 500 bytes for stack */
}

SECTIONS
{
  .text :
  {
    PROVIDE(__text_start = .);

    *(.progmem.gcc*)
    *(.progmem*)
    . = ALIGN(2);

    *(.trampolines*)
    *(.jumptables*)
    *(.lowtext*)

    *(.init0)
    KEEP (*(.init0))
    *(.init1)
    KEEP (*(.init1))
    *(.init2)
    KEEP (*(.init2))
    *(.init3)
    KEEP (*(.init3))
    *(.init4)
    KEEP (*(.init4))
    *(.init5)
    KEEP (*(.init5))
    *(.init6)
    KEEP (*(.init6))
    *(.init7)
    KEEP (*(.init7))
    *(.init8)
    KEEP (*(.init8))

    *(.text.main)
    KEEP (*(.text.main))
    *(.text)
    *(.text.*)

    *(.rf4ce) 
    KEEP (*(.rf4ce))
    *(.rf4ce.*)
    KEEP (*(.rf4ce.*))

    PROVIDE(__text_end = .);
  } > text

  .data : AT (ADDR(.text) + SIZEOF(.text))
  {
    PROVIDE(__data_start = .);
    *(.data*)
    *(.rodata*)
    *(.gnu.linkonce.d*)
    . = ALIGN(2);
    PROVIDE(__data_end = .);
  } > data

  .bss __data_end :
  {
    PROVIDE(__bss_start = .);
    *(.bss*)
    *(COMMON)
    PROVIDE(__bss_end = .);
  } > data

  .noinit __bss_end :
  {
    *(.noinit*)
    PROVIDE(__heap_start = .);
  } > data

  .access_sect :
  {
    KEEP(*(.access_sect*))
    *(.access_sect*)
  } > access

  __data_load_start = LOADADDR(.data);
  __data_load_end = __data_load_start + SIZEOF(.data);

  /DISCARD/ :
  {
    *(.init9)
    *(.fini*)
  }

}
