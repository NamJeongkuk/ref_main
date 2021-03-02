/* Linker script to configure memory regions. */
MEMORY
{
  DEBUG_VECTORS (rx)          : ORIGIN = 0x00000000, LENGTH = 0x40
  ROM_REGISTERS (rx)          : ORIGIN = 0x00000400, LENGTH = 0x100
  BOOT_LOADER (rx)            : ORIGIN = 0x00000500, LENGTH = 0x6000 - ORIGIN(ROM_REGISTERS) - LENGTH(ROM_REGISTERS)
  HEADER (r)                  : ORIGIN = 0x00006000, LENGTH = 0x40
  VECTORS (rx)                : ORIGIN = 0x00006100, LENGTH = 0x400
  FLASH (rx)                  : ORIGIN = 0x00006500, LENGTH = 0x0200000 - ORIGIN(VECTORS) - LENGTH(VECTORS)
  RAM (rwx)                   : ORIGIN = 0x1FFE0000, LENGTH = 0x00A0000  /* 640K */
  DATA_FLASH (rx)             : ORIGIN = 0x40100000, LENGTH = 0x0010000  /*  64K */
  QSPI_FLASH (rx)             : ORIGIN = 0x60000000, LENGTH = 0x4000000  /*  64M, Change in QSPI section below also */
  SDRAM (rwx)                 : ORIGIN = 0x90000000, LENGTH = 0x2000000  /*  32M */
  ID_CODE (rx)                : ORIGIN = 0x0100A150, LENGTH = 0x10    /* 16 bytes */
}

/* Library configurations */

ENTRY(Reset_Handler)

SECTIONS
{
   .debugVectors :
   {
       KEEP(*(.debug_vectors*))
   } > DEBUG_VECTORS

   .romRegisters :
   {
      . = ALIGN(4);
      KEEP(*(.rom_registers*))
   } > ROM_REGISTERS

   .bootLoader :
   {
      _bootLoaderStart = .;
      __BootLoader_Location = .;
      KEEP(*(.bootLoader))
   } > BOOT_LOADER

   .header :
   {
      . = ALIGN(4);
      _bootLoaderEnd = . - 1;
      _headerStart = .;
      KEEP(*(.header))
      _headerEnd = . - 1;
      _romStart = .;
   } > HEADER

   .vectors :
   {
      __Vectors_Start = .;
      KEEP(*(.fixed_vectors*))
      KEEP(*(.application_vectors*))
      __Vectors_End = .;
   } > VECTORS

    .text :
    {
        *(.text*)

        KEEP(*(.version))
        KEEP(*(.init))
        KEEP(*(.fini))

        *(.rodata*)

        KEEP(*(.eh_frame*))

        __ROM_End = .;
    } > FLASH

    __Vectors_Size = __Vectors_End - __Vectors;

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

    /* If DTC is used, put the DTC vector table at the start of SRAM.
       This avoids memory holes due to 1K alignment required by it. */
    .fsp_dtc_vector_table (NOLOAD) :
    {
        . = ORIGIN(RAM);
        __Dtc_Vector_Table = .;
        *(.fsp_dtc_vector_table)
    } > RAM

    /* Initialized data section. */
    .data :
    {
        __data_start__ = .;
        . = ALIGN(4);

        __Code_In_RAM_Start = .;

        KEEP(*(.code_in_ram*))
        __Code_In_RAM_End = .;

        *(vtable)
        *(.data.*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);


        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))

        . = ALIGN(4);

        /* All data end */
        __data_end__ = .;

    } > RAM AT > FLASH

    .noinit (NOLOAD):
    {
        . = ALIGN(4);
        __noinit_start = .;
        KEEP(*(.noinit*))
        __noinit_end = .;
    } > RAM

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
   } > RAM AT > FLASH

    /* Stacks are stored in this section. */
    .stack_dummy (NOLOAD):
    {
        . = ALIGN(8);
        __StackLimit = .;
        /* Main stack */
        KEEP(*(.stack))
        __StackTop = .;
        /* Thread stacks */
        KEEP(*(.stack*))
        __StackTopAll = .;
    } > RAM

    PROVIDE(__stack = __StackTopAll);

    /* This symbol represents the end of user allocated RAM. The RAM after this symbol can be used
       at run time for things such as ThreadX memory pool allocations. */
    __RAM_segment_used_end__ = ALIGN(__StackTopAll , 4);

    /* Data flash. */
    .data_flash :
    {
        __Data_Flash_Start = .;
        KEEP(*(.data_flash*))
        __Data_Flash_End = .;
    } > DATA_FLASH

    .id_code :
    {
        __ID_Code_Start = .;
        KEEP(*(.id_code*))
        __ID_Code_End = .;
    } > ID_CODE

   /* Helper variables for accessing sections from c code. */
   __StackTop = __stackCheckEnd;
   __StackBottom = ORIGIN(RAM) + LENGTH(RAM);
   __Vectors_Start_Address = __Vectors_Start;
   __Vectors_End_Address = __Vectors_End - 1;
   __Boot_Header = ORIGIN(BOOT_LOADER);
   __Header = _headerStart;
   __Flash_End_Address = ORIGIN(FLASH) + LENGTH(FLASH) - 1;
   _codeEnd = __Flash_End_Address;
}
