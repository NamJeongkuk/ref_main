local tools = require 'tools.lua-erd-documentation-tools.lua-erd-documentation-tools'
local Types = require 'build.rx130.GeneratedTypes'

tools.build_json({
  namespace = 'rx130',
  address = 0xC0,
  parsers = {
    tools.parser.standard
  },
  erd_files = {
    'src/Application/DataSource/SystemErds.h'
  },
  types_factory = Types,
  output = 'build/rx130/doc/erd-definitions.json'
})
