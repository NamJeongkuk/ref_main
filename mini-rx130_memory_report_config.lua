return {
  Application = {
    ram = {
      map = 'build/mini-rx130/mini-rx130.map',
      type = 'rx-gcc',
      total = 48 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/mini-rx130/mini-rx130.napl',
      apl = 'build/mini-rx130/mini-rx130.apl',
      endianness = 'little'
    }
  }
}
