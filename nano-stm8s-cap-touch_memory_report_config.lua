return {
  BootLoader = {
    ram = {
      map = 'lib/stm8-boot-loader/build/boot-loader.map',
      type = 'sdcc',
      total = 1 * 1024
    },
    rom = {
      map = 'lib/stm8-boot-loader/build/boot-loader.map',
      type = 'sdcc',
      total = 1 * 1024 + 64
    }
  },
  Application = {
    ram = {
      map = 'build/nano-stm8s-cap-touch/nano-stm8s-cap-touch.map',
      type = 'sdcc',
      total = 2 * 1024
    },
    rom = {
      map = 'build/nano-stm8s-cap-touch/nano-stm8s-cap-touch.map',
      type = 'sdcc',
      total = (32 * 1024) - (1024 + 64)
    }
  }
}
