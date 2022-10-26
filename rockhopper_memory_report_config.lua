return {
  ['Boot Loader'] = {
    ram = {
      map = 'lib/rockhopper-boot-loader/build/rockhopper-boot-loader/rockhopper-boot-loader.map',
      type = 'rx-gcc',
      total = 48 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'lib/rockhopper-boot-loader/build/rockhopper-boot-loader/rockhopper-boot-loader.napl',
      apl = 'lib/rockhopper-boot-loader/build/rockhopper-boot-loader/rockhopper-boot-loader.apl',
      endianness = 'little'
    }
  },
  Application = {
    ram = {
      map = 'build/rockhopper/rockhopper.map',
      type = 'rx-gcc',
      total = 48 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/rockhopper/rockhopper.napl',
      apl = 'build/rockhopper/rockhopper.apl',
      endianness = 'little'
    }
  }
}
