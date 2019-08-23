return {
  Application = {
    ram = {
      map = 'build/bonzalez/bonzalez.map',
      type = 'sdcc',
      total = 1 * 1024,
      sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/bonzalez/bonzalez.napl',
      apl = 'build/bonzalez/bonzalez.apl',
      endianness = 'little'
    }
  }
}
