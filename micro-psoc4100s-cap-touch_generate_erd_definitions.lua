local tools = require 'tools.lua-erd-documentation-tools.lua-erd-documentation-tools'
local Types = require 'build.micro-psoc4100s-cap-touch.GeneratedTypes'

tools.build_json({
  namespace = 'micro-psoc4100s-cap-touch',
  address = 0xB3,
  parsers = {
    tools.parser.standard
  },
  erd_files = {
    'src/micro-psoc4100s-cap-touch/DataSource/SystemErds.h'
  },
  types_factory = Types,
  output = 'build/micro-psoc4100s-cap-touch/doc/erd-definitions.json'
})
