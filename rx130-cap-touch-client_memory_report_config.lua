return {
  Application = {
    ram = {
      map = 'build/rx130-cap-touch-client/rx130-cap-touch-client.map',
      type = 'rx-gcc',
      total = 48 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'build/rx130-cap-touch-client/rx130-cap-touch-client.napl',
      apl = 'build/rx130-cap-touch-client/rx130-cap-touch-client.apl',
      endianness = 'little'
    }
  }
}
