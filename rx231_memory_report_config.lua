return {
  ['Boot Loader'] = {
    ram = {
      map = 'lib/boot-loaders/build/rx231-boot-loader/rx231-boot-loader.map',
      type = 'rx-gcc',
      total = 64 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'lib/boot-loaders/build/rx231-boot-loader/rx231-boot-loader.napl',
      apl = 'lib/boot-loaders/build/rx231-boot-loader/rx231-boot-loader.apl',
      endianness = 'little'
    }
  },
  Application = {
    ram = {
      map = 'build/rx231/rx231.map',
      type = 'rx-gcc',
      total = 64 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/rx231/rx231.napl',
      apl = 'build/rx231/rx231.apl',
      endianness = 'little'
    }
  }
}
