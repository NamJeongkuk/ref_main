MEMORY
{
	RAM (rx) : ORIGIN = 0x4, LENGTH = 0x7FFC
	APPLICATION (rx) : ORIGIN = 0xFFFC0000, LENGTH = 0x35800
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
		__headerStart = .;
		KEEP(*(.header))
		*(.text)
		*(.text.*)
		*(P)
		. = ALIGN(4);
		__relocatableVectorTableStart = .;
		KEEP(*(.relocatableVectorTable))
		__relocatableVectorTableEnd = .;
	} > APPLICATION
	.rodata :
	{
		*(.rodata)
		*(.rodata.*)
		*(C_1)
		*(C_2)
		*(C)
		_mdata = .;
		_etext = .;
	} > APPLICATION
	.parametricData :
	{
		*(.parametricData)
	} > PARAMETRIC_DATA
	.fvectors :
	{
		. = ALIGN(4);
		__fixedVectorTableStart = .;
		KEEP(*(.fvectors))
		__fixedVectorTableEnd = .;
	} > FIXED_VECTOR_TABLE
	.dtcVectorTable :
	{
		KEEP(*(.dtcVectorTable))
	} > DTC_VECTOR_TABLE
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
		. = ALIGN(4);
		_edata = .;
	} > RAM AT>APPLICATION
	.bss :
	{
		_bss = .;
		. = ALIGN(4);
		*(.bss)
		*(.bss.*)
		*(COMMON)
		*(B)
		*(B_1)
		*(B_2)
		_ebss = .;
		__end = .;
	} > RAM
	__stackCheckRomStart = LOADADDR(.stackCheck);
	.stackCheck :
	{
		__stackCheckStart = .;
		*(.stackCheck)
		__stackCheckEnd = .;
	} > RAM AT>APPLICATION

	_ustack = ORIGIN(RAM) + LENGTH(RAM) - 4;
	_istack = ORIGIN(RAM) + LENGTH(RAM) - 4;
	__istackSize = _istack - _ebss;
}
