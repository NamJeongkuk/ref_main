local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local map = require 'lua-common'.fp.map
local grid_keys = require 'Grid.GridKeys'

return function(core)
  import(core)

  local grid_key_index = 1

  local function get_grid_keys()
    if grid_key_index == 1 then
      return grid_keys.deltas
    elseif grid_key_index == 2 then
      return grid_keys.fresh_food_grid
    else
      return grid_keys.freezer_grid
    end
  end

  local function length_of_table(t)
    local length = 0

    for k in pairs(t) do length = length + 1 end
    return length
  end

  local unpack_gridline = function(value)
    return value;
  end

  local function kv_unpack(data, keys)
    local ret_table = {}
    local tkeys = {}
    local t = keys

    for k in pairs(t) do table.insert(tkeys, k) end
    table.sort(tkeys)
    for _, k in ipairs(tkeys) do table.insert(ret_table, data[t[k]]) end

    grid_key_index = grid_key_index + 1
    return ret_table
  end

  local unpack_table = function(data)
    return structure(u8(length_of_table(data)), pointer(structure(table.unpack(map(kv_unpack(data, get_grid_keys()), unpack_gridline)))));
  end

  local generate = memoize(function(config)
    grid_key_index = 1 -- resets to 1

    return TypedString(
      { 'grid_deltas' },
      structure(
        u8(length_of_table(config.deltas)),
        pointer(
          structure(table.unpack(map(kv_unpack(config.deltas, get_grid_keys()), unpack_table)))
        )
      )
    )
  end)

  return function(config)
    validate_arguments(config, {
      deltas = {
        constraint.table_keys({
          fresh_food_grid_deltas = {constraint.table_keys({
            fresh_food_no_freeze_limit = { constraint.typed_string('gridline') },
            fresh_food_low_hyst = { constraint.typed_string('gridline') },
            fresh_food_low_hyst_plus_delta = { constraint.typed_string('gridline') },
            fresh_food_high_hyst = { constraint.typed_string('gridline') },
            fresh_food_extra_high_hyst = { constraint.typed_string('gridline') },
            fresh_food_super_high_hyst = { constraint.typed_string('gridline') }
          })},
          freezer_grid_deltas = {constraint.table_keys({
            freezer_low_hyst = { constraint.typed_string('gridline') },
            freezer_delta = { constraint.typed_string('gridline') },
            freezer_high_hyst = { constraint.typed_string('gridline') },
            freezer_extra_high_hyst = { constraint.typed_string('gridline') },
            freezer_super_high_hyst = { constraint.typed_string('gridline') },
            freezer_extreme_high_hyst = { constraint.typed_string('gridline') }
          })}
        })
      }
    })
    return generate(config)
  end
end
