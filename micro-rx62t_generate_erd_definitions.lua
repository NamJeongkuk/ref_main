local tools = require 'tools.lua-erd-documentation-tools.lua-erd-documentation-tools'
local Types = require 'build.micro-rx62t.GeneratedTypes'

tools.build_json({
  namespace = 'micro-rx62t',
  address = 0xBA,
  parsers = {
    tools.parser.standard
  },
  erd_files = {
    'src/MicroApplication/DataSource/MicroSystemErds.h'
  },
  types_factory = Types,
  output = 'build/micro-rx62t/doc/erd-definitions.json'
})
