return {
  Application = {
    ram = {
      map = 'build/nano-rl78g12/nano-rl78g12.map',
      type = 'rl78-gcc',
      total = 1.5 * 1024,
      ram_sections = {
        'RAM'
      }
    },
    rom = {
      map = 'build/nano-rl78g12/nano-rl78g12.map',
      type = 'rl78-gcc',
      total = 16 * 1024,
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
