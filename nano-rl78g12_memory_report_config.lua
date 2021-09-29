local build_tools = ({
  ['kpit-rl78'] = {
    type = 'rl78-gcc',
    sections = {
      ram = { 'RAM' },
      rom = { 'VEC', 'IVEC', 'OPT', 'SEC_ID', 'OCDSTAD', 'ROM' }
    }
  },

  ['llvm-rl78'] = {
    type = 'llvm-rl78',
    sections = {
      ram = { 'bss', 'data' },
      rom = { 'vec', 'vects', 'option_bytes', 'security_id', 'lowtext', 'tors', 'text', 'rodata', 'frodata' }
    }
  }
})[os.getenv('BUILD_TOOLS')]

return {
  Application = {
    ram = {
      map = 'build/nano-rl78g12/nano-rl78g12.map',
      type = build_tools.type,
      total = math.floor(1.5 * 1024),
      sections = build_tools.sections.ram
    },
    rom = {
      map = 'build/nano-rl78g12/nano-rl78g12.map',
      type = build_tools.type,
      total = 16 * 1024,
      sections = build_tools.sections.rom
    }
  }
}
