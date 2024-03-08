local function extract_path(require_path)
  return require_path:match("@(.*[/\\])") or './'
end

local my_path = extract_path(debug.getinfo(1).source) .. '/'
package.path = my_path .. 'tools/lua-nv-erd-tools/src/?.lua;' .. package.path
package.path = my_path .. 'tools/lua-nv-erd-tools/lib/?/?.lua;' .. package.path

local tools = require 'tools.lua-nv-erd-tools.lua-nv-erd-tools'
local target = os.getenv('TARGET')
local output_dir = os.getenv('OUTPUT_DIR')
local system_erds = os.getenv('SYSTEM_ERDS')
local Types = require(output_dir .. '.GeneratedTypes')

local writes_per_year_index = tonumber(os.getenv('NV_USAGE_REPORT_WRITES_PER_YEAR_INDEX'))
local erd_memory_type_index = tonumber(os.getenv('NV_USAGE_REPORT_ERD_MEMORY_TYPE_INDEX'))
local erd_types = os.getenv('NV_USAGE_REPORT_ERD_TYPES')
local product_life = tonumber(os.getenv('NV_USAGE_REPORT_PRODUCT_LIFE'))
local maximum_erase_count = tonumber(os.getenv('NV_USAGE_REPORT_MAXIMUM_ERASE_COUNT'))
local eeprom_size = tonumber(os.getenv('NV_USAGE_REPORT_EEPROM_SIZE'))
local write_alignment = tonumber(os.getenv('NV_USAGE_REPORT_WRITE_ALIGNMENT'))

local function split(s)
  local split = {}
  for chunk in s:gmatch('%S+') do
    table.insert(split, chunk)
  end
  return split
end

local config = {
  namespace = target,
  types_factory = Types,
  output = output_dir .. '/' .. target .. '_nv_erd_list.lua',
  erd_files = { system_erds },
  writes_per_year_index = writes_per_year_index,
  erd_memory_type_index = erd_memory_type_index,
  number_of_backups = 1,
  erd_memory_type = split(erd_types),
  algorithm_type = 'eeprom',
  product_life = product_life,
  maximum_erase_count = maximum_erase_count,
  maximum_used_percentage_after_block_transfer = 80,
  eeprom_size = eeprom_size,
  write_alignment = write_alignment
}

tools.generate_nv_erd_list(config)
tools.generate_report(output_dir .. '/' .. target .. '_nv_erd_list', output_dir .. '/' .. target .. '_nv_usage_report.md')
