return {
  Application = {
    ram = {
      map = 'build/rx231/rx231.map',
      type = 'rx-gcc',
      total = 64 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/rx231/rx231.napl',
      apl = 'build/rx231/rx231.apl',
      endianness = 'little'
    }
  }
}
