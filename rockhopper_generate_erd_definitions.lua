local tools = require 'tools.lua-erd-documentation-tools.lua-erd-documentation-tools'
local Types = require 'build.rockhopper.GeneratedTypes'

tools.build_json({
  namespace = 'mb',
  address = 0xC0,
  parsers = {
    tools.parser.standard
  },
  erd_files = {
    'src/Application/DataSource/SystemErds.h'
  },
  types_factory = Types,
  output = 'build/rockhopper/doc/erd-definitions.json'
})
