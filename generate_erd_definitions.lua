local tools = require 'tools.lua-erd-documentation-tools.lua-erd-documentation-tools'
local Types = require 'build.GeneratedTypes'

tools.build_json({
  namespace = 'bonzalez',
  address = 0xB2,
  parsers = {
    tools.parser.standard
  },
  erd_files = {
    'src/Application/DataSource/SystemErds.h'
  },
  types_factory = Types,
  output = 'build/bonzalez/doc/erd-definitions.json'
})
