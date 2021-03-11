return {
  ['Boot Loader'] = {
    ram = {
      map = 'lib/boot-loaders/build/small-rl78g13-boot-loader/small-rl78g13-boot-loader.map',
      type = 'rl78-gcc',
      total = 8 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'lib/boot-loaders/build/small-rl78g13-boot-loader/small-rl78g13-boot-loader.napl',
      apl = 'lib/boot-loaders/build/small-rl78g13-boot-loader/small-rl78g13-boot-loader.apl',
      endianness = 'little'
    }
  },
  Application = {
    ram = {
      map = 'build/micro-rl78g13/micro-rl78g13.map',
      type = 'rl78-gcc',
      total = 8 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/micro-rl78g13/micro-rl78g13.napl',
      apl = 'build/micro-rl78g13/micro-rl78g13.apl',
      endianness = 'little'
    }
  }
}
