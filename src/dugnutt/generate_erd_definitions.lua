local tools = require 'Tools.lua-erd-documentation-tools.lua-erd-documentation-tools'
local Types = require 'Tools.Types'

tools.build_json({
  namespace = 'dugnutt',
  address = 0x99,
  parsers = {
    tools.parser.standard
  },
  erd_files = {
    'src/dugnutt/Application/DataModel/SystemErds.h'
  },
  types_factory = Types,
  output = 'build/dugnutt/doc/erd-definitions.json'
})
