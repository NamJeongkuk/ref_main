return {
  Application = {
    ram = {
      map = 'build/rx130/rx130.map',
      type = 'rx-gcc',
      total = 48 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/rx130/rx130.napl',
      apl = 'build/rx130/rx130.apl',
      endianness = 'little'
    }
  }
}
