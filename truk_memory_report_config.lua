return {
  Application = {
    ram = {
      map = 'build/truk/dugnutt.map',
      type = 'rx-gcc',
      total = 16 * 1024,
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
