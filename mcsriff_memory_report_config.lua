return {
  Application = {
    ram = {
      map = 'build/mcsriff/mcsriff.map',
      type = 'arm-gcc',
      total = 32 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/mcsriff/mcsriff.napl',
      apl = 'build/mcsriff/mcsriff.apl',
      endianness = 'little'
    }
  }
}
