return {
  ['Boot Loader'] = {
    ram = {
      map = 'lib/boot-loaders/build/small-psoc4100s-boot-loader/small-psoc4100s-boot-loader.map',
      type = 'arm-gcc',
      total = 16 * 1024,
      ram_sections = {
        'ram'
      }
    },
    rom = {
      srec = 'lib/boot-loaders/build/small-psoc4100s-boot-loader/small-psoc4100s-boot-loader.napl',
      apl = 'lib/boot-loaders/build/small-psoc4100s-boot-loader/small-psoc4100s-boot-loader.apl',
      type = 'arm-gcc',
      total = 16 * 1024,
      endianness = 'little'
    }
  },
  Application = {
    ram = {
      map = 'build/micro-psoc4100s-cap-touch/micro-psoc4100s-cap-touch.map',
      type = 'arm-gcc',
      total = 16 * 1024,
      ram_sections = {
        'ram'
      }
    },
    rom = {
      srec = 'build/micro-psoc4100s-cap-touch/micro-psoc4100s-cap-touch.napl',
      apl = 'build/micro-psoc4100s-cap-touch/micro-psoc4100s-cap-touch.apl',
      type = 'arm-gcc',
      total = 112 * 1024,
      endianness = 'little'
    }
  }
}
