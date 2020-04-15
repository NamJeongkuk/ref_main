return {
  BootLoader = {
    ram = {
      map = 'lib/boot-loaders/build/micro-psoc4100s-cap-touch-boot-loader/micro-psoc4100s-cap-touch-boot-loader.map',
      type = 'sdcc',
      total = 2 * 1024
    },
    rom = {
      map = 'lib/boot-loaders/build/micro-psoc4100s-cap-touch-boot-loader/micro-psoc4100s-cap-touch-boot-loader.map',
      type = 'sdcc',
      total = 8 * 1024
    }
  },
  Application = {
    ram = {
      map = 'build/micro-psoc4100s-cap-touch/micro-psoc4100s-cap-touch.map',
      type = 'sdcc',
      total = 2 * 1024
    },
    rom = {
      map = 'build/micro-psoc4100s-cap-touch/micro-psoc4100s-cap-touch.map',
      type = 'sdcc',
      total = 24 * 1024
    }
  }
}
