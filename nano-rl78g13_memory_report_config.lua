return {
  Application = {
    ram = {
      map = 'build/nano-rl78g13/nano-rl78g13.map',
      type = 'rl78-gcc',
      total = 4 * 1024,
      sections = {
        'RAM'
      }
    },
    rom = {
      map = 'build/nano-rl78g13/nano-rl78g13.map',
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
