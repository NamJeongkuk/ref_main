return {
  Application = {
    ram = {
      map = 'build/nano-rl78g13/nano-rl78g13.map',
      type = 'llvm-rl78',
      total = 4 * 1024,
      sections = {
        'bss',
        'data'
      }
    },
    rom = {
      map = 'build/nano-rl78g13/nano-rl78g13.map',
      type = 'llvm-rl78',
      total = 64 * 1024,
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
