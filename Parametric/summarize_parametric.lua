if not arg[1] or not arg[2] then
  print('usage: lua ' .. arg[0] .. ' path/to/parametric.lua path/to/output.ini')
  os.exit(-1)
end

package.path = 'lib/lua-common/?.lua;' .. package.path

local printed = {}

local function printer(s)
  table.insert(printed, s)
end

local pretty_print = require 'lua-common'.util.PrettyPrint(printer)
local write_to_file = require 'lua-common'.io.write_to_file
local ingest = require 'src.ingest'

local ingested = ingest(arg[1])

pretty_print(ingested)

write_to_file(arg[2], table.concat(printed, '\n'))
