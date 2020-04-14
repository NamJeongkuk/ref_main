return {
  BootLoader = {
    ram = {
      map = 'lib/boot-loaders/build/mini-rx130-boot-loader/mini-rx130-boot-loader.map',
      type = 'rx-gcc',
      total = 48 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'lib/boot-loaders/build/mini-rx130-boot-loader/mini-rx130-boot-loader.napl',
      apl = 'lib/boot-loaders/build/mini-rx130-boot-loader/mini-rx130-boot-loader.apl',
      endianness = 'little'
    }
  },
  Application = {
    ram = {
      map = 'build/mini-rx130/mini-rx130.map',
      type = 'rx-gcc',
      total = 48 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/mini-rx130/mini-rx130.napl',
      apl = 'build/mini-rx130/mini-rx130.apl',
      endianness = 'little'
    }
  }
}
