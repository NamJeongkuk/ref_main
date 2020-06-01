return {
  Application = {
    ram = {
      map = 'build/nano-stm32g0/nano-stm32g0.map',
      type = 'arm-gcc',
      total = 32 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/nano-stm32g0/nano-stm32g0.napl',
      apl = 'build/nano-stm32g0/nano-stm32g0.apl',
      endianness = 'little'
    }
  }
}
