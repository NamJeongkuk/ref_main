return {
  ['Boot Loader'] = {
    ram = {
      map = 'lib/boot-loaders/build/mini-stm32f3-boot-loader/mini-stm32f3-boot-loader.map',
      type = 'arm-gcc',
      total = 12 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'lib/boot-loaders/build/mini-stm32f3-boot-loader/mini-stm32f3-boot-loader.napl',
      apl = 'lib/boot-loaders/build/mini-stm32f3-boot-loader/mini-stm32f3-boot-loader.apl',
      endianness = 'little'
    }
  },
  Application = {
    ram = {
      map = 'build/mini-stm32f3/mini-stm32f3.map',
      type = 'arm-gcc',
      total = 32 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/mini-stm32f3/mini-stm32f3.napl',
      apl = 'build/mini-stm32f3/mini-stm32f3.apl',
      endianness = 'little'
    }
  }
}
