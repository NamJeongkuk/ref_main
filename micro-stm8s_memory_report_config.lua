return {
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
