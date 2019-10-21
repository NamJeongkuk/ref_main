MEMORY
{
  DEBUG (rx)       : ORIGIN = 0x08000000, LENGTH = 0x08
  BOOT_HEADER (r)  : ORIGIN = 0x08000194, LENGTH = 0x40
  HEADER (r)       : ORIGIN = 0x08005000, LENGTH = 0x40
  VECTORS (rx)     : ORIGIN = 0x08005100, LENGTH = 0x194
  FLASH (rx)       : ORIGIN = 0x08005294, LENGTH = 128K - 20K - LENGTH(VECTORS) - LENGTH(HEADER) - 4K
  USER_FLASH (rw)  : ORIGIN = 0x0801F000, LENGTH = 4K
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

	.userFlash :
	{
		__User_Flash = .;
		KEEP(*(.userFlash))
	} > USER_FLASH

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
   __User_Flash = ORIGIN(USER_FLASH);
   __StackTop = ORIGIN(RAM) + LENGTH(RAM);
	__StackSize = __StackTop - __bss_end__;
}
