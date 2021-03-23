local tools = require 'tools.lua-erd-documentation-tools.lua-erd-documentation-tools'
local Types = require 'build.nano-rl78g12.GeneratedTypes'

tools.build_json(
  {
    namespace = 'nano-rl78g12',
    address = 0xB7,
    parsers = {
      tools.parser.standard
    },
    erd_files = {
      'src/NanoApplication/DataSource/NanoSystemErds.h'
    },
    types_factory = Types,
    output = 'build/nano-rl78g12/doc/erd-definitions.json'
  }
)
