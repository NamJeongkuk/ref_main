return {
  Application = {
    ram = {
      map = 'build/mcscriff/mcscriff.map',
      type = 'arm-gcc',
      total = 12 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/mcscriff/mcscriff.napl',
      apl = 'build/mcscriff/mcscriff.apl',
      endianness = 'little'
    }
  }
}
