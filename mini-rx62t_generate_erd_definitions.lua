local tools = require 'tools.lua-erd-documentation-tools.lua-erd-documentation-tools'
local Types = require 'build.mini-rx62t.GeneratedTypes'

tools.build_json({
  namespace = 'mini-rx62t',
  address = 0xBA,
  parsers = {
    tools.parser.standard
  },
  erd_files = {
    'src/Application/DataSource/SystemErds.h'
  },
  types_factory = Types,
  output = 'build/mini-rx62t/doc/erd-definitions.json'
})
