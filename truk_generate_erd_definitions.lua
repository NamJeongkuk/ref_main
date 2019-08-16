local tools = require 'tools.lua-erd-documentation-tools.lua-erd-documentation-tools'
local Types = require 'build.generated_types'

tools.build_json({
  namespace = 'truk',
  address = 0xB1,
  parsers = {
    tools.parser.standard
  },
  erd_files = {
    'src/Application/DataSource/SystemErds.h'
  },
  types_factory = Types,
  output = 'build/dugnutt/doc/erd-definitions.json'
})
