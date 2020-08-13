return {
  ['Boot Loader'] = {
    ram = {
      map = 'lib/boot-loaders/build/small-stm8s-boot-loader/small-stm8s-boot-loader.map',
      type = 'sdcc',
      total = 2 * 1024
    },
    rom = {
      map = 'lib/boot-loaders/build/small-stm8s-boot-loader/small-stm8s-boot-loader.map',
      type = 'sdcc',
      total = 8 * 1024
    }
  },
  Application = {
    ram = {
      map = 'build/micro-stm8s/micro-stm8s.map',
      type = 'sdcc',
      total = 2 * 1024
    },
    rom = {
      map = 'build/micro-stm8s/micro-stm8s.map',
      type = 'sdcc',
      total = 24 * 1024
    }
  }
}
