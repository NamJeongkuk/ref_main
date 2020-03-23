return {
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
