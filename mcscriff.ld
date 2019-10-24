MEMORY
{
  DEBUG (rx)       : ORIGIN = 0x08000000, LENGTH = 0x08
  BOOT_HEADER (r)  : ORIGIN = 0x08000194, LENGTH = 0x40
  NVDATA_1 (rw)    : ORIGIN = 0x08005000, LENGTH = 2K
  NVDATA_2 (rw)    : ORIGIN = 0x08005800, LENGTH = 2K
  HEADER (r)       : ORIGIN = 0x08006000, LENGTH = 0x40
  VECTORS (rx)     : ORIGIN = 0x08006100, LENGTH = 0x194
  FLASH (rx)       : ORIGIN = 0x08006300, LENGTH = 128K - 20K - LENGTH(VECTORS) - LENGTH(HEADER) - LENGTH(NVDATA_1) - LENGTH(NVDATA_2)
  RAM (rwx)        : ORIGIN = 0x20000000, LENGTH = 32K
  CCMRAM (rwx)     : ORIGIN = 0x10000000, LENGTH = 4K
}

ENTRY(Reset_Handler)

SECTIONS
{
	.debug_vector_table :
	{
		KEEP(*(.debug_vector_table))
	} > DEBUG

	.nv_data_1 :
   {
      . = ALIGN(4);
      __NvData1_Location = .;
      KEEP(*(.nv_data_1))
      . = ALIGN(4);
   } > NVDATA_1

   .nv_data_2 :
   {
      . = ALIGN(4);
      __NvData2_Location = .;
      KEEP(*(.nv_data_2))
      . = ALIGN(4);
   } > NVDATA_2

	.header :
	{
		__Header_Location = .;
		KEEP(*(.header))
	} > HEADER

	.vectors :
	{
		__Vectors_Start = .;
		KEEP(*(.vectors))
		__Vectors_End = .;
	} > VECTORS

	.text :
	{
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
   __Boot_Header = ORIGIN(BOOT_HEADER);
   __Header = ORIGIN(HEADER);
   __StackTop = ORIGIN(RAM) + LENGTH(RAM);
	__StackSize = __StackTop - __bss_end__;
}
