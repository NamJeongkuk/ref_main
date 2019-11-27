return {
  Application = {
    ram = {
      map = 'build/bonzalez/bonzalez.map',
      type = 'sdcc',
      total = 2 * 1024
    },
    rom = {
      map = 'build/bonzalez/bonzalez.map',
      type = 'sdcc',
      total = (8 * 1024) - (1024 + 64)
    }
  }
}
