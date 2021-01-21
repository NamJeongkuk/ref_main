local tools = require 'tools.lua-erd-documentation-tools.lua-erd-documentation-tools'
local Types = require 'build.micro-rl78.GeneratedTypes'

tools.build_json(
  {
    namespace = 'micro-rl78',
    address = 0xB7,
    parsers = {
      tools.parser.standard
    },
    erd_files = {
      'src/MicroApplication/DataSource/MicroSystemErds.h'
    },
    types_factory = Types,
    output = 'build/micro-rl78/doc/erd-definitions.json'
  }
)
