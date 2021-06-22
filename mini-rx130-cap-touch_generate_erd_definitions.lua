local tools = require 'tools.lua-erd-documentation-tools.lua-erd-documentation-tools'
local target = os.getenv('TARGET')
local Types = require('build.' .. target .. '.GeneratedTypes')

tools.build_json({
  namespace = target,
  address = 0xB0,
  parsers = {
    tools.parser.standard
  },
  erd_files = {
    'src/' .. target .. '/DataSource/SystemErds.h'
  },
  types_factory = Types,
  output = 'build/' .. target ..'/doc/erd-definitions.json'
})
