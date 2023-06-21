local tools = require 'tools.lua-erd-documentation-tools.lua-erd-documentation-tools'
local Types = require 'build.rockhopper.GeneratedTypes'
local rockhopper_post_processing = require 'post_processing.rockhopper_modify_winning_vote_erds'

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
  output = 'build/rockhopper/doc/temp-erd-definitions.json'
})

local file = io.open('build/rockhopper/doc/temp-erd-definitions.json', 'r')
local json_string = file:read("*all")
file:close()

rockhopper_post_processing.rockhopper_modify_winning_vote_erds(json_string, 'build/rockhopper/doc/erd-definitions.json')
