local tools = require 'tools.lua-erd-documentation-tools.lua-erd-documentation-tools'
local Types = require 'build.mini-rx130.GeneratedTypes'

tools.build_json({
  namespace = 'mini-rx130',
  address = 0xB0,
  parsers = {
    tools.parser.standard
  },
  erd_files = {
    'src/Application/DataSource/SystemErds.h'
  },
  types_factory = Types,
  output = 'build/mini-rx130/doc/erd-definitions.json'
})
