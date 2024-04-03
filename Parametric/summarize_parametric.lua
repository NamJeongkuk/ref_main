if not arg[1] or not arg[2] then
  print('usage: lua ' .. arg[0] .. ' path/to/parametric.lua path/to/output.ini')
  os.exit(-1)
end

package.path = 'lib/lua-common/?.lua;' .. package.path

local pairs_sorted_by_keys = require 'lua-common'.util.pairs_sorted_by_keys
local printed = {}

local function printer(s)
  table.insert(printed, s)
end

local function string_ends_with(s, substring)
  return (string.sub(s, 0 - #substring) == substring)
end

local function PrettyTable(print)
  local function pretty_print(t, header)
    if not header then header = "" end
    local subHeader = header

    if type(t) == 'table' then
      for k, v in pairs_sorted_by_keys(t) do
        local formatting = header;
        local subHeaderIsPresent = string_ends_with(header, tostring(k) .. ".")

        if not subHeaderIsPresent then
          formatting = formatting .. k
        end

        if type(v) == 'table' then
          subHeader = subHeaderIsPresent and formatting or formatting .. '.'
          pretty_print(v, subHeader)
        else
          print(formatting .. ": " .. tostring(v))
        end
      end
    else
      print(tostring(t))
    end
  end

  return pretty_print
end

local write_to_file = require 'lua-common'.io.write_to_file
local pretty_print = PrettyTable(printer)
local ingest = require 'ingest'

local ingested = ingest(arg[1])
pretty_print(ingested)

write_to_file(arg[2], table.concat(printed, '\n'))