return {
  Application = {
    ram = {
      map = 'build/stm8s/stm8s.map',
      type = 'sdcc',
      total = 1 * 1024
    },
    rom = {
      map = 'build/stm8s/stm8s.map',
      type = 'sdcc',
      total = (8 * 1024) - (1024 + 64)
    }
  }
}
