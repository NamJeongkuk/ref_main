return {
  Application = {
    ram = {
      map = 'build/nano-rl78g12/nano-rl78g12.map',
      type = 'llvm-rl78',
      total = math.floor(1.5 * 1024),
      sections = {
        'bss',
        'data'
      }
    },
    rom = {
      map = 'build/nano-rl78g12/nano-rl78g12.map',
      type = 'llvm-rl78',
      total = 16 * 1024,
      sections = {
        'vec',
        'vects',
        'option_bytes',
        'security_id',
        'lowtext',
        'tors',
        'text',
        'rodata',
        'frodata'
      }
    }
  }
}
