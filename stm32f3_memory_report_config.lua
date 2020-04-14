return {
  BootLoader = {
    ram = {
      map = 'lib/boot-loaders/build/stm32f3-boot-loader/stm32f3-boot-loader.map',
      type = 'arm-gcc',
      total = 12 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'lib/boot-loaders/build/stm32f3-boot-loader/stm32f3-boot-loader.napl',
      apl = 'lib/boot-loaders/build/stm32f3-boot-loader/stm32f3-boot-loader.apl',
      endianness = 'little'
    }
  },
  Application = {
    ram = {
      map = 'build/stm32f3/stm32f3.map',
      type = 'arm-gcc',
      total = 32 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/stm32f3/stm32f3.napl',
      apl = 'build/stm32f3/stm32f3.apl',
      endianness = 'little'
    }
  }
}
