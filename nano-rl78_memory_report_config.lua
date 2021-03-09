return {
  Application = {
    ram = {
      map = 'build/nano-rl78/nano-rl78.map',
      type = 'rl78-gcc',
      total = 4 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      map = 'build/nano-rl78/nano-rl78.map',
      type = 'rl78-gcc',
      total = 64 * 1024,
      sections = {
        'VEC',
        'IVEC',
        'OPT',
        'SEC_ID',
        'OCDSTAD',
        'ROM'
      }
    }
  }
}
