return {
  Application = {
    ram = {
      map = 'build/wesrey/wesrey.map',
      type = 'rx-gcc',
      total = 48 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/wesrey/wesrey.napl',
      apl = 'build/wesrey/wesrey.apl',
      endianness = 'little'
    }
  }
}
