return {
  ['Boot Loader'] = {
    ram = {
      map = 'lib/boot-loaders/build/rx130-boot-loader/rx130-boot-loader.map',
      type = 'rx-gcc',
      total = 48 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'lib/boot-loaders/build/rx130-boot-loader/rx130-boot-loader.napl',
      apl = 'lib/boot-loaders/build/rx130-boot-loader/rx130-boot-loader.apl',
      endianness = 'little'
    }
  },
  Application = {
    ram = {
      map = 'build/rx130/rx130.map',
      type = 'rx-gcc',
      total = 48 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/rx130/rx130.napl',
      apl = 'build/rx130/rx130.apl',
      endianness = 'little'
    }
  }
}
