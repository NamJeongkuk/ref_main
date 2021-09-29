MEMORY
{
   VEC : ORIGIN = 0x0, LENGTH = 4
   IVEC : ORIGIN = 0x4, LENGTH = 188
   OPT : ORIGIN = 0xC0, LENGTH = 4
   SEC_ID : ORIGIN = 0xC4, LENGTH = 10
   OCDSTAD : ORIGIN = 0xCE, LENGTH = 10
   OCDROM : ORIGIN = 0x17E00, LENGTH = 512
   BOOT_ROM : ORIGIN = 0xD8, LENGTH = 0x3400 - 0xD8
   ROM : ORIGIN = 0x3400, LENGTH = 0x8000
   MIRROR : ORIGIN = 0xF3000, LENGTH = 44800
   VECTOR_TABLE_CONFIGURATION : ORIGIN = 0xFDF00, LENGTH = 6 /* Must have an even size */
   RAM : ORIGIN = 0xFDF06, LENGTH = 0xFFEDB - 0xFDF06 + 1
   UL_RAM_CHECK_BUFFER : ORIGIN = 0xFFEDC, LENGTH = 4
}

SECTIONS
{
   .vec 0x0: AT(0x0)
   {
      KEEP(*(.vec))
   } > VEC

   .vects 0x4: AT(0x4)
   {
      KEEP(*(.vects))
   } > IVEC

   .option_bytes 0xC0: AT(0xC0)
   {
      KEEP(*(.option_bytes))
   } > OPT

   .security_id 0xC4: AT(0xC4)
   {
      KEEP(*(.security_id))
   } > SEC_ID

   .lowtext 0xD8: AT(0xD8)
   {
      *(.plt)
      *(.lowtext)
      . = ALIGN(2);
   } > BOOT_ROM

   .header :
   {
      KEEP(*(.header))
   } > ROM

   .tors :
   {
      __CTOR_LIST__ = .;
      . = ALIGN(2);
      ___ctors = .;
      KEEP(*(.ctors))
      ___ctors_end = .;
      __CTOR_END__ = .;
      __DTOR_LIST__ = .;
      ___dtors = .;
      KEEP(*(.dtors))
      ___dtors_end = .;
      __DTOR_END__ = .;
      . = ALIGN(2);
      _mdata = .;
   } > ROM

   .text (. + __romdatacopysize):
   {
      *(.text)
      *(.text.*)
      etext = .;
      . = ALIGN(2);
   } > ROM

   PROVIDE(__rl78_abs__ = 0);

   .init :
   {
      *(.init)
   } > ROM

   .fini :
   {
      KEEP(*(.fini))
   } > ROM

   .got :
   {
      *(.got)
      *(.got.plt)
   } > ROM

   .rodata MAX(., 0x3000):
   {
      . = ALIGN(2);
      *(.rodata)
      *(.rodata.*)
      _erodata = .;
   } > ROM

   .frodata :
   {
      . = ALIGN(2);
      *(.frodata)
      *(.frodata.*)
      _efrodata = .;
   } > ROM

   .eh_frame_hdr :
   {
      *(.eh_frame_hdr)
   } > ROM

   .eh_frame :
   {
      *(.eh_frame)
   } > ROM

   .jcr :
   {
      *(.jcr)
      . = ALIGN(2);
   } > ROM

   .vectorTableConfiguration (NOLOAD) :
   {
      /* llvm-rl78 uses __data to find the start of RAM */
      __data = .;

      __vtor = .;
      . += 2;
      __br = .;
      . += 1;
      __isr_lsb = .;
      . += 1;
      __isr_msb = .;
      . += 1;
      __align = .;
   } > VECTOR_TABLE_CONFIGURATION

   .data ORIGIN(RAM): AT(_mdata)
   {
      . = ALIGN(2);
      _data = .;
      *(.data)
      *(.data.*)
      . = ALIGN(2);
      _edata = .;
   } > RAM

   PROVIDE(__romdatacopysize = SIZEOF(.data));

   .bss (NOLOAD):
   {
      . = ALIGN(2);
      _bss = .;
      *(.bss)
      *(.bss.**)
      . = ALIGN(2);
      *(COMMON)
      . = ALIGN(2);
      _ebss = .;
      _end = .;
   } > RAM

   .stack_sentinel (NOLOAD):
   {
      . = ALIGN(2);
      *(.stack_sentinel)
   } > RAM

   .ul_ram_check_buffer (NOLOAD):
   {
      *(.ul_ram_check_buffer)
   } > UL_RAM_CHECK_BUFFER

   PROVIDE(stack_size = 0x1000);
   PROVIDE(stack_start = ORIGIN(RAM) + LENGTH(RAM) - 1);

   __ApplicationStart = ORIGIN(ROM);
   __ApplicationEnd = ORIGIN(ROM) + LENGTH(ROM) - 1;
   __Header = __ApplicationStart;

   __UlRamCheckStart = ORIGIN(VECTOR_TABLE_CONFIGURATION);
   __UlRamCheckEnd = ORIGIN(VECTOR_TABLE_CONFIGURATION) + LENGTH(VECTOR_TABLE_CONFIGURATION) + LENGTH(RAM) - 1;
   __UlRamCheckStepSize = LENGTH(UL_RAM_CHECK_BUFFER);

   .stack stack_start (NOLOAD) : AT(stack_start)
   {
      _stack = stack_start;
      ASSERT((_stack > (_end + stack_size)), "Error: Too much data - no room left for the stack");
   } > RAM
}
