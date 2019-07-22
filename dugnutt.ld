MEMORY
{
	RAM (rx) : ORIGIN = 0x4, LENGTH = 0x3FFC
	DTC_VECTOR_TABLE (rx) : ORIGIN = 0xFFFF5000, LENGTH = 0xAF7F
	FIXED_VECTOR_TABLE (rx) : ORIGIN = 0xFFFFFF80, LENGTH = 0x80
	ROM (rx) : ORIGIN = 0xFFFE0000, LENGTH = 0x14FFF
}
OUTPUT_ARCH(rx)
ENTRY(_PowerON_Reset)

SECTIONS
{
	.text :
	{
		. = ALIGN(4);
		__bootLoaderHeaderStart = .;
		KEEP(*(.bootLoaderHeader))
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
	.fvectors :
	{
		. = ALIGN(4);
		__fixedVectorTableStart = .;
		KEEP(*(.fvectors))
		__fixedVectorTableEnd = .;
	} > FIXED_VECTOR_TABLE
	.data :
	{
		_data = .;
		__dtcVectorTableStart = .;
		*(.DtcTable)
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

	_ustack = ORIGIN(RAM) + LENGTH(RAM) - 4;
	_istack = ORIGIN(RAM) + LENGTH(RAM) - 4;
	__iStackSize = _istack - _ebss;
}
