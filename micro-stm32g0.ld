MEMORY
{
  DEBUG (rx)       : ORIGIN = 0x08000000, LENGTH = 0x08
  HEADER (r)       : ORIGIN = 0x08003000, LENGTH = 0x40
  VECTORS (rx)     : ORIGIN = 0x08003200, LENGTH = 0xB8
  FLASH (rx)       : ORIGIN = ORIGIN(VECTORS) + LENGTH(VECTORS), LENGTH = 0x08020000 - ORIGIN(FLASH)
  RAM (rwx)        : ORIGIN = 0x20000000, LENGTH = 32K
}

ENTRY(Reset_Handler)

SECTIONS
{
   .debug_vector_table :
   {
      KEEP(*(.debug_vector_table))
   } > DEBUG

   .header :
   {
      __Header_Location = .;
      KEEP(*(.header))
   } > HEADER

   .vectors :
   {
      . = ALIGN(4);
      __Vectors_Start = .;
      KEEP(*(.vectors))
      . = ALIGN(4);
      __Vectors_End = .;
   } > VECTORS

   .text :
   {
      . = ALIGN(4);
      *(.text*)
      KEEP(*(.init))
      KEEP(*(.fini))
      *(.rodata*)
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

   __etext = .;

   __data_rom_start__ = LOADADDR(.data);

   .data :
   {
      . = ALIGN(4);
      __data_start__ = .;
      *(.data*)
      . = ALIGN(4);
      __data_end__ = .;
   } > RAM AT>FLASH

   .bss :
   {
      . = ALIGN(4);
      __bss_start__ = .;
      *(.bss*)
      *(COMMON)
      . = ALIGN(4);
      __bss_end__ = .;
   } > RAM

   .stackCheck :
   {
      __stackCheckStart = .;
      *(.stackCheck)
      __stackCheckEnd = .;
   } > RAM AT>FLASH

   /* Helper variables for accessing sections from c code. */
   __Header = ORIGIN(HEADER);
   __ApplicationStart = __Header;
   __ApplicationEnd = ORIGIN(FLASH) + LENGTH(FLASH) - 1;
   __StackTop = __stackCheckEnd;
   __StackBottom = ORIGIN(RAM) + LENGTH(RAM);
   __StackSize = __StackBottom - __bss_end__;
}
