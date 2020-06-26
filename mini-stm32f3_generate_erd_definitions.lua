local tools = require 'tools.lua-erd-documentation-tools.lua-erd-documentation-tools'
local Types = require 'build.mini-stm32f3.GeneratedTypes'

tools.build_json(
  {
    namespace = 'mini-stm32f3',
    address = 0xB4,
    parsers = {
      tools.parser.standard
    },
    erd_files = {
      'src/Application/DataSource/SystemErds.h'
    },
    types_factory = Types,
    output = 'build/mini-stm32f3/doc/erd-definitions.json'
  }
)
