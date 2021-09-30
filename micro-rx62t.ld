MEMORY
{
	RAM (rx) : ORIGIN = 0x10, LENGTH = 16 * 1024 - 0x10
	ROM (rx) : ORIGIN = 0xFFFC0000, LENGTH = 0xFFFFCC00 - 0xFFFC0000
	DTC_VECTOR_TABLE (rx) : ORIGIN = 0xFFFFCC00, LENGTH = 0x400
	BOOT_ROM (rx) : ORIGIN = 0xFFFFD000, LENGTH = 0xFFFFFF80 - 0xFFFFD000
	FIXED_VECTOR_TABLE (rx) : ORIGIN = 0xFFFFFF80, LENGTH = 0x80
}
OUTPUT_ARCH(rx)
ENTRY(_PowerON_Reset)

SECTIONS
{
	.text :
	{
		. = ALIGN(4);
		__applicationHeaderStart = .;
		KEEP(*(.applicationHeader))
		*(.text)
		*(.text.*)
		*(P)
		. = ALIGN(4);
		__relocatableVectorTableStart = .;
		KEEP(*(.relocatableVectorTable))
	} > ROM

	.rodata :
	{
		*(.rodata)
		*(.rodata.*)
		*(C_1)
		*(C_2)
		*(C)
		_mdata = .;
		_etext = .;
	} > ROM

	.dtcVectorTable :
	{
		KEEP(*(.dtcVectorTable))
	} > DTC_VECTOR_TABLE

	.fixedVectorTable :
	{
		. = ALIGN(4);
		__fixedVectorTableStart = .;
		KEEP(*(.fixedVectorTable))
		__fixedVectorTableEnd = .;
	} > FIXED_VECTOR_TABLE

	.data :
	{
		_data = .;
		__dtcVectorTableStart = .;
		*(.dtcTransferInformation)
		. = ALIGN(4);
		__dtcVectorTableEnd = .;
		*(.data)
		*(.data.*)
		*(D)
		*(D_1)
		*(D_2)
	} > RAM AT>ROM

	.flashCode :
	{
		*(.flashCode)
		_edata = .;
	}  > RAM AT>ROM

	.restartString :
	{
		*(.restartString)
	} > RAM

	.bss :
	{
		_bss = .;
		*(.bss)
		*(.bss.*)
		*(COMMON)
		*(B)
		*(B_1)
		*(B_2)
		_ebss = .;
		_end = .;
	} > RAM

	.stackCheck :
	{
		__stackCheckStart = .;
		*(.stackCheck)
		__stackCheckEnd = .;
		__stackEnd = .;
	} > RAM AT>ROM

	/*
    * This ensures that a full 256 byte write is completed at the end of the data by
	 * making sure that the second 128 bytes is not all 0xFFs
	 */
	.hack :
	{
		FILL(0x55);
		. = ALIGN(256);
		BYTE(0xFF);
	} > ROM

	_applicationStart = ORIGIN(ROM);
	_applicationEnd = ORIGIN(DTC_VECTOR_TABLE) + LENGTH(DTC_VECTOR_TABLE) - 1;

	_bootLoaderStart = ORIGIN(BOOT_ROM);
	_bootLoaderEnd = ORIGIN(FIXED_VECTOR_TABLE) + LENGTH(FIXED_VECTOR_TABLE) - 1;

	_ustack = ORIGIN(RAM) + LENGTH(RAM) - 4;
	_istack = ORIGIN(RAM) + LENGTH(RAM) - 4;

   __stackStart = _ustack;
}
