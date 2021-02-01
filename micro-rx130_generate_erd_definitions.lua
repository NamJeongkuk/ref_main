local tools = require 'tools.lua-erd-documentation-tools.lua-erd-documentation-tools'
local Types = require 'build.micro-rx130.GeneratedTypes'

tools.build_json({
  namespace = 'micro-rx130',
  address = 0xB0,
  parsers = {
    tools.parser.standard
  },
  erd_files = {
    'src/MicroApplication/DataSource/MicroSystemErds.h'
  },
  types_factory = Types,
  output = 'build/micro-rx130/doc/erd-definitions.json'
})
