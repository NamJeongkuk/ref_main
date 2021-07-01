return {
  ['Boot Loader'] = {
    ram = {
      map = 'lib/boot-loaders/build/small-rx130-boot-loader/small-rx130-boot-loader.map',
      type = 'rx-gcc',
      total = 48 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'lib/boot-loaders/build/small-rx130-boot-loader/small-rx130-boot-loader.napl',
      apl = 'lib/boot-loaders/build/small-rx130-boot-loader/small-rx130-boot-loader.apl',
      endianness = 'little'
    }
  },
  Application = {
    ram = {
      map = 'build/micro-rx130-cap-touch/micro-rx130-cap-touch.map',
      type = 'rx-gcc',
      total = 48 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/micro-rx130-cap-touch/micro-rx130-cap-touch.napl',
      apl = 'build/micro-rx130-cap-touch/micro-rx130-cap-touch.apl',
      endianness = 'little'
    }
  }
}