INCLUDE lib/rockhopper-boot-loader/rockhopper-boot-loader-memory.ld

OUTPUT_ARCH(rx)
ENTRY(_PowerON_Reset)

SECTIONS
{
   __bootLoaderHeaderStart = BootLoaderHeaderAddress;

   __applicationStart = ORIGIN(APPLICATION_ROM);
   __applicationEnd = ORIGIN(APPLICATION_ROM) + LENGTH(APPLICATION_ROM) - 1;
   __applicationHeaderStart = __applicationStart;

   __parametricStart = ORIGIN(PARAMETRIC_ROM);
   __parametricEnd = ORIGIN(PARAMETRIC_ROM) + LENGTH(PARAMETRIC_ROM) - 1;
   __parametricHeaderStart = __parametricStart;

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
   } > APPLICATION_ROM

   .rodata :
   {
      *(.rodata)
      *(.rodata.*)
      *(C_1)
      *(C_2)
      *(C)
      _mdata = .;
      _etext = .;
   } > APPLICATION_ROM

   .bss (NOLOAD):
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
   } > RAM AT>APPLICATION_ROM

   .flashCode :
   {
      *(.flashCode)
      _edata = .;
   }  > RAM AT>APPLICATION_ROM

   .stackCheck :
   {
      __stackCheckStart = .;
      *(.stackCheck)
      __stackCheckEnd = .;
      __stackEnd = .;
   } > RAM AT>APPLICATION_ROM

   .dtcVectorTable :
   {
      . = ALIGN(1024);
      KEEP(*(.dtcVectorTable))
   } > APPLICATION_ROM

   .parametricData :
   {
      *(.parametric_data)
   } > PARAMETRIC_ROM

   .fixedVectorTable FixedVectorTableAddress :
   {
      __fixedVectorTableStart = .;
      KEEP(*(.fixedVectorTable))
      __fixedVectorTableEnd = .;
   } > BOOT_LOADER_ROM

   _ustack = ORIGIN(RAM) + LENGTH(RAM) - 4;
   _istack = ORIGIN(RAM) + LENGTH(RAM) - 4;
   __iStackSize = _istack - _ebss;

   __stackStart = _ustack;
}
