return {
  BootLoader = {
    ram = {
      map = 'lib/boot-loaders/build/mini-rx231-boot-loader/mini-rx231-boot-loader.map',
      type = 'rx-gcc',
      total = 64 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'lib/boot-loaders/build/mini-rx231-boot-loader/mini-rx231-boot-loader.napl',
      apl = 'lib/boot-loaders/build/mini-rx231-boot-loader/mini-rx231-boot-loader.apl',
      endianness = 'little'
    }
  },
  Application = {
    ram = {
      map = 'build/mini-rx231/mini-rx231.map',
      type = 'rx-gcc',
      total = 64 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/mini-rx231/mini-rx231.napl',
      apl = 'build/mini-rx231/mini-rx231.apl',
      endianness = 'little'
    }
  }
}
