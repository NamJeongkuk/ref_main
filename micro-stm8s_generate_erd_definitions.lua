local tools = require 'tools.lua-erd-documentation-tools.lua-erd-documentation-tools'
local Types = require 'build.micro-stm8s.GeneratedTypes'

tools.build_json({
  namespace = 'micro-stm8s',
  address = 0xB3,
  parsers = {
    tools.parser.standard
  },
  erd_files = {
    'src/micro-stm8s/DataSource/SystemErds.h'
  },
  types_factory = Types,
  output = 'build/micro-stm8s/doc/erd-definitions.json'
})
