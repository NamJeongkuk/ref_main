local tools = require 'tools.lua-erd-documentation-tools.lua-erd-documentation-tools'
local Types = require 'build.GeneratedTypes'

tools.build_json({
  namespace = 'mini-rx231',
  address = 0xB1,
  parsers = {
    tools.parser.standard
  },
  erd_files = {
    'src/Application/DataSource/SystemErds.h'
  },
  types_factory = Types,
  output = 'build/mini-rx231/doc/erd-definitions.json'
})
