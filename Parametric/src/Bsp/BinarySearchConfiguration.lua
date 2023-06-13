local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local ErdListToSortedErdMappings = require 'Bsp.ErdListToSortedErdMappings'

local element_size = 4
local member_size = 2
local offset = 2
local key_is_signed = false

return function(core)
  import(core)

  local generate = memoize(function(config)
    local erd_list_to_sorted_erd_mappings = ErdListToSortedErdMappings(core)
    local erd_mappings = erd_list_to_sorted_erd_mappings({ erd_list = config.erd_list})
    if #erd_mappings > 0 then
      return TypedString(
        { 'binary_search_configuration' },
        pointer(
          structure(
            #erd_mappings > 0 and pointer(structure(table.unpack(erd_mappings))) or null(),
            u16(#erd_mappings),
            u16(element_size),
            u16(member_size),
            u16(offset),
            bool(key_is_signed)
          )
        )
      )
    else
      return TypedString(
        { 'binary_search_configuration' },
        pointer(
          structure(
            #erd_mappings > 0 and pointer(structure(table.unpack(erd_mappings))) or null(),
            u16(#erd_mappings),
            u16(0),
            u16(0),
            u16(0),
            bool(false)
          )
        )
      )
    end
  end)

  return function(config)
    validate_arguments(
      config,
      {
        erd_list = { constraint.array_elements({ constraint.type('table') }) }
      }
    )
    return generate(config)
  end
end
