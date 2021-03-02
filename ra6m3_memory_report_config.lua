return {
  ['Boot Loader'] = {
    ram = {
      map = 'lib/boot-loaders/build/medium-ra6m3-boot-loader/medium-ra6m3-boot-loader.map',
      type = 'arm-gcc',
      total = 640 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'lib/boot-loaders/build/medium-ra6m3-boot-loader/medium-ra6m3-boot-loader.napl',
      apl = 'lib/boot-loaders/build/medium-ra6m3-boot-loader/medium-ra6m3-boot-loader.apl',
      endianness = 'little'
    }
  },
  Application = {
    ram = {
      map = 'build/ra6m3/ra6m3.map',
      type = 'arm-gcc',
      total = 640 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/ra6m3/ra6m3.napl',
      apl = 'build/ra6m3/ra6m3.apl',
      endianness = 'little'
    }
  }
}
