return {
  Application = {
    ram = {
      map = 'build/dugnutt/dugnutt.map',
      type = 'rx-gcc',
      total = 16 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/dugnutt/dugnutt.napl',
      apl = 'build/dugnutt/dugnutt.apl',
      endianness = 'little'
    }
  }
}
