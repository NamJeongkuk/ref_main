local tools = require 'tools.lua-erd-documentation-tools.lua-erd-documentation-tools'
local Types = require 'build.ra6m3.GeneratedTypes'

tools.build_json(
  {
    namespace = 'ra6m3',
    address = 0xB8,
    parsers = {
      tools.parser.standard
    },
    erd_files = {
      'src/Application/DataSource/SystemErds.h'
    },
    types_factory = Types,
    output = 'build/ra6m3/doc/erd-definitions.json'
  }
)
