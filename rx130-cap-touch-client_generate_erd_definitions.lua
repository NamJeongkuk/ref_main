local tools = require 'tools.lua-erd-documentation-tools.lua-erd-documentation-tools'
local Types = require 'build.GeneratedTypes'

tools.build_json(
  {
    namespace = 'rx130-cap-touch-client',
    address = 0xB0,
    parsers = {
      tools.parser.standard
    },
    erd_files = {
      'src/Application/DataSource/SystemErds.h'
    },
    types_factory = Types,
    output = 'build/rx130-cap-touch-client/doc/erd-definitions.json'
  }
)
