local tools = require 'tools.lua-erd-documentation-tools.lua-erd-documentation-tools'
local Types = require 'build.micro-stm32g0.GeneratedTypes'

tools.build_json(
  {
    namespace = 'micro-stm32g0',
    address = 0xB6,
    parsers = {
      tools.parser.standard
    },
    erd_files = {
      'src/MicroApplication/DataSource/MicroSystemErds.h'
    },
    types_factory = Types,
    output = 'build/micro-stm32g0/doc/erd-definitions.json'
  }
)
