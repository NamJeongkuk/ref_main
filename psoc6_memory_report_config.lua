return {
  ['Boot Loader'] = {
    ram = {
      map = 'lib/boot-loaders/build/medium-psoc6-boot-loader/medium-psoc6-boot-loader.map',
      type = 'arm-gcc',
      total = 1024 * 1024,
      ram_sections = {
        'ram'
      }
    },
    rom = {
      srec = 'lib/boot-loaders/build/medium-psoc6-boot-loader/medium-psoc6-boot-loader.napl',
      apl = 'lib/boot-loaders/build/medium-psoc6-boot-loader/medium-psoc6-boot-loader.apl',
      endianness = 'little'
    }
  },

  Application = {
    ram = {
      map = 'build/psoc6/psoc6.map',
      type = 'arm-gcc',
      total = 1024 * 1024,
      ram_sections = {
        'ram'
      }
    },
    rom = {
      srec = 'build/psoc6/psoc6.napl',
      apl = 'build/psoc6/psoc6.apl',
      endianness = 'little',
      header_address = 0x100082E0
    }
  }
}
