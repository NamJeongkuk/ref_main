return {
  Application = {
    ram = {
      map = 'build/mini-dugnutt/mini-dugnutt.map',
      type = 'rx-gcc',
      total = 48 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/mini-dugnutt/mini-dugnutt.napl',
      apl = 'build/mini-dugnutt/mini-dugnutt.apl',
      endianness = 'little'
    }
  }
}
