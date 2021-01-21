return {
  ['Boot Loader'] = {
    ram = {
      map = 'lib/boot-loaders/build/small-rl78-boot-loader/small-rl78-boot-loader.map',
      type = 'rl78-gcc',
      total = 8 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'lib/boot-loaders/build/small-rl78-boot-loader/small-rl78-boot-loader.napl',
      apl = 'lib/boot-loaders/build/small-rl78-boot-loader/small-rl78-boot-loader.apl',
      endianness = 'little'
    }
  },
  Application = {
    ram = {
      map = 'build/micro-rl78/micro-rl78.map',
      type = 'rl78-gcc',
      total = 8 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/micro-rl78/micro-rl78.napl',
      apl = 'build/micro-rl78/micro-rl78.apl',
      endianness = 'little'
    }
  }
}
