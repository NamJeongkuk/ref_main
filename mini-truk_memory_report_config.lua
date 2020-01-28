return {
  Application = {
    ram = {
      map = 'build/mini-truk/mini-truk.map',
      type = 'rx-gcc',
      total = 64 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/mini-truk/mini-truk.napl',
      apl = 'build/mini-truk/mini-truk.apl',
      endianness = 'little'
    }
  }
}
