return {
  ['Boot Loader'] = {
    ram = {
      map = 'lib/boot-loaders/build/small-stm32g0-boot-loader/small-stm32g0-boot-loader.map',
      type = 'arm-gcc',
      total = 32 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'lib/boot-loaders/build/small-stm32g0-boot-loader/small-stm32g0-boot-loader.napl',
      apl = 'lib/boot-loaders/build/small-stm32g0-boot-loader/small-stm32g0-boot-loader.apl',
      endianness = 'little'
    }
  },
  Application = {
    ram = {
      map = 'build/micro-stm32g0/micro-stm32g0.map',
      type = 'arm-gcc',
      total = 32 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/micro-stm32g0/micro-stm32g0.napl',
      apl = 'build/micro-stm32g0/micro-stm32g0.apl',
      endianness = 'little'
    }
  }
}
