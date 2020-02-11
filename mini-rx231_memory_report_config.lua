return {
  Application = {
    ram = {
      map = 'build/mini-rx231/mini-rx231.map',
      type = 'rx-gcc',
      total = 64 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/mini-rx231/mini-rx231.napl',
      apl = 'build/mini-rx231/mini-rx231.apl',
      endianness = 'little'
    }
  }
}
