return {
  Application = {
    ram = {
      map = 'build/truk/truk.map',
      type = 'rx-gcc',
      total = 64 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/truk/truk.napl',
      apl = 'build/truk/truk.apl',
      endianness = 'little'
    }
  }
}
