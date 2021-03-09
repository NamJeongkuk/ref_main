local tools = require 'tools.lua-erd-documentation-tools.lua-erd-documentation-tools'
local Types = require 'build.nano-rl78.GeneratedTypes'

tools.build_json(
  {
    namespace = 'nano-rl78',
    address = 0xB7,
    parsers = {
      tools.parser.standard
    },
    erd_files = {
      'src/NanoApplication/DataSource/NanoSystemErds.h'
    },
    types_factory = Types,
    output = 'build/nano-rl78/doc/erd-definitions.json'
  }
)
