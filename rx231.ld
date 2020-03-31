MEMORY
{
	RAM (rx) : ORIGIN = 0x10, LENGTH = 0xFFF0
	ROM (rx) : ORIGIN = 0xFFF80000, LENGTH = 0x76800
	DTC_VECTOR_TABLE (rx) : ORIGIN = 0xFFFF5800, LENGTH = 0x400
	PARAMETRIC_DATA (r) : ORIGIN = 0xFFFF6000, LENGTH = 0x5000
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
		*(.dtcVectorTable)
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
	} > RAM AT>ROM

	_ustack = ORIGIN(RAM) + LENGTH(RAM) - 4;
	_istack = ORIGIN(RAM) + LENGTH(RAM) - 4;
	__iStackSize = _istack - _ebss;
}
