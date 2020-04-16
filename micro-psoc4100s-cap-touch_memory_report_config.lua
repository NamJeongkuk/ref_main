return {
  BootLoader = {
    ram = {
      map = 'lib/boot-loaders/build/micro-psoc4100s-boot-loader/micro-psoc4100s-boot-loader.map',
      type = 'sdcc',
      total = 8 * 1024
    },
    rom = {
      map = 'lib/boot-loaders/build/micro-psoc4100s-boot-loader/micro-psoc4100s-boot-loader.map',
      type = 'sdcc',
      total = 16 * 1024
    }
  },
  Application = {
    ram = {
      map = 'build/micro-psoc4100s-cap-touch/micro-psoc4100s-cap-touch.map',
      type = 'sdcc',
      total = 8 * 1024
    },
    rom = {
      map = 'build/micro-psoc4100s-cap-touch/micro-psoc4100s-cap-touch.map',
      type = 'sdcc',
      total = 48 * 1024
    }
  }
}
