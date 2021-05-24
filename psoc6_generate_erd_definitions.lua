local tools = require 'tools.lua-erd-documentation-tools.lua-erd-documentation-tools'
local Types = require 'build.psoc6.GeneratedTypes'

tools.build_json(
  {
    namespace = 'psoc6',
    address = 0xB9,
    parsers = {
      tools.parser.standard
    },
    erd_files = {
      'src/Application/DataSource/SystemErds.h'
    },
    types_factory = Types,
    output = 'build/psoc6/doc/erd-definitions.json'
  }
)
