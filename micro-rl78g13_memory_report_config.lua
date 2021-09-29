local build_tools = ({
  ['kpit-rl78'] = {
    type = 'rl78-gcc',
    sections = {
      ram = { 'RAM' }
    }
  },

  ['llvm-rl78'] = {
    type = 'llvm-rl78',
    sections = {
      ram = { 'bss', 'data' }
    }
  }
})[os.getenv('BUILD_TOOLS')]

return {
  ['Boot Loader'] = {
    ram = {
      map = 'lib/boot-loaders/build/small-rl78g13-boot-loader/small-rl78g13-boot-loader.map',
      type = 'rl78-gcc',
      total = 8 * 1024,
      sections = {
        'RAM'
      }
    },
    rom = {
      srec = 'lib/boot-loaders/build/small-rl78g13-boot-loader/small-rl78g13-boot-loader.napl',
      apl = 'lib/boot-loaders/build/small-rl78g13-boot-loader/small-rl78g13-boot-loader.apl',
      endianness = 'little'
    }
  },
  Application = {
    ram = {
      map = 'build/micro-rl78g13/micro-rl78g13.map',
      type = build_tools.type,
      total = 8 * 1024,
      sections = build_tools.sections.ram
    },
    rom = {
      srec = 'build/micro-rl78g13/micro-rl78g13.napl',
      apl = 'build/micro-rl78g13/micro-rl78g13.apl',
      endianness = 'little'
    }
  }
}
