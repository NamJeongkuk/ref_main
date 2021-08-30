return {
  ['Boot Loader'] = {
    ram = {
      map = 'lib/boot-loaders/build/small-rx62t-boot-loader/small-rx62t-boot-loader.map',
      type = 'rx-gcc',
      total = 16 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'lib/boot-loaders/build/small-rx62t-boot-loader/small-rx62t-boot-loader.napl',
      apl = 'lib/boot-loaders/build/small-rx62t-boot-loader/small-rx62t-boot-loader.apl',
      endianness = 'little'
    }
  },
  Application = {
    ram = {
      map = 'build/micro-rx62t/micro-rx62t.map',
      type = 'rx-gcc',
      total = 16 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/micro-rx62t/micro-rx62t.napl',
      apl = 'build/micro-rx62t/micro-rx62t.apl',
      endianness = 'little'
    }
  }
}
