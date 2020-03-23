return {
  Application = {
    ram = {
      map = 'build/nano-stm8s/nano-stm8s.map',
      type = 'sdcc',
      total = 1 * 1024
    },
    rom = {
      map = 'build/nano-stm8s/nano-stm8s.map',
      type = 'sdcc',
      total = (8 * 1024) - (1024 + 64)
    }
  }
}
