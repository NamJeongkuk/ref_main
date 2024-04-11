local tools = require('tools/lua-erd-documentation-tools/lua-erd-documentation-tools')
local Types = require('build/rockhopper/GeneratedTypes')
local jkjson = require('tools/jkjson/jkjson')

local json_path = './build/rockhopper/doc/erd-definitions.json'

tools.build_json({
  namespace = 'mb',
  address = 0xC0,
  parsers = {
    tools.parser.standard,
  },
  erd_files = {
    'src/Application/DataSource/SystemErds.h',
  },
  types_factory = Types,
  output = json_path,
})

local function compact_vote_erds(erds)
  local function is_a_vote_erd(erd)
    for _, value in ipairs(erd.data) do
      if (value.values or {})['1'] == 'Vote_Care' and not erd.name:match('.*ResolvedVote$') then
        return true
      end
    end
  end

  for i = 1, #erds do
    local erd = erds[i]
    if erd.name:match('.*WinningVote.*') then
      local vote_erds = {}
      while is_a_vote_erd(erds[i + 1]) do
        i = i + 1
        local name = 'WinningVoteErd_' .. erds[i].name
        vote_erds[name] = name
      end
      for j, value in pairs(erd.data[1].values) do
        erd.data[1].values[j] = vote_erds[value]
      end
    end
  end
end

local definitions = jkjson.load(json_path)
compact_vote_erds(definitions.erds)
jkjson.dump(json_path, definitions, {
  indent = 2,
  sort_keys = true,
})
