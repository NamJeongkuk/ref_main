return {
  Application = {
    ram = {
      map = 'build/stm32f3/stm32f3.map',
      type = 'arm-gcc',
      total = 32 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/stm32f3/stm32f3.napl',
      apl = 'build/stm32f3/stm32f3.apl',
      endianness = 'little'
    }
  }
}
