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
  local config_is_one_dimension = true
  local max_supported_grid_lines_per_delta = 7

  local function get_grid_keys()
    if config_is_one_dimension then
      if grid_key_index == 1 then
        return grid_keys.one_dimensional_grid_deltas
      end
    else
      if grid_key_index == 1 then
        return grid_keys.two_dimensional_grid_deltas
      elseif grid_key_index == 2 then
        return grid_keys.fresh_food_grid
      else
        return grid_keys.freezer_grid
      end
    end
  end

  local function length_of_table(t)
    local length = 0

    for k in pairs(t) do length = length + 1 end

    if config_is_one_dimension and length > max_supported_grid_lines_per_delta then
      length = max_supported_grid_lines_per_delta
    end

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

  local get_all_nested_gridlines = function(data)
    local gridlines = {}
    local modes_per_grid = map(kv_unpack(data, grid_keys.one_dimensional_grid_delta_modes), unpack_gridline)
    local number_of_gridlines_per_mode = 0

    for _, modes in ipairs(modes_per_grid) do
      local gridlines_per_mode = map(kv_unpack(modes, grid_keys.one_dimensional_mode_grid), unpack_gridline)
      for _, gridline in ipairs(gridlines_per_mode) do
        number_of_gridlines_per_mode = #gridlines_per_mode
        table.insert(gridlines, gridline)
      end
    end

    return number_of_gridlines_per_mode, gridlines
  end

  local unpack_table = function(data)
    if config_is_one_dimension then
      local number_of_gridlines, all_gridlines = get_all_nested_gridlines(data)
      return structure(u8(number_of_gridlines), pointer(structure(table.unpack(all_gridlines))))
    else
      return structure(u8(length_of_table(data)), pointer(structure(table.unpack(map(kv_unpack(data, get_grid_keys()), unpack_gridline)))));
    end
  end

  local generate = memoize(function(config)
    grid_key_index = 1 -- resets to 1
    local dimensions = length_of_table(config.deltas)
    config_is_one_dimension = (dimensions == 1)

    return TypedString(
      { 'grid_deltas' },
      structure(
        u8(dimensions),
        pointer(
          structure(table.unpack(map(kv_unpack(config.deltas, get_grid_keys()), unpack_table)))
        )
      )
    )
  end)

  return function(config)
    validate_arguments(config, {
      deltas = {
        constraint.ored({
          constraint.table_keys({
            grid_deltas = { constraint.table_keys({
              mode_1 = { constraint.table_keys({
                gridline_1 = { constraint.typed_string('gridline') },
                gridline_2 = { constraint.typed_string('gridline') },
                gridline_3 = { constraint.typed_string('gridline') },
                gridline_4 = { constraint.typed_string('gridline') },
                gridline_5 = { constraint.typed_string('gridline') },
                gridline_6 = { constraint.typed_string('gridline') },
                gridline_7 = { constraint.typed_string('gridline') },
              })}
            },
            {
              mode_2 = { constraint.table_keys({
                gridline_1 = { constraint.typed_string('gridline') },
                gridline_2 = { constraint.typed_string('gridline') },
                gridline_3 = { constraint.typed_string('gridline') },
                gridline_4 = { constraint.typed_string('gridline') },
                gridline_5 = { constraint.typed_string('gridline') },
                gridline_6 = { constraint.typed_string('gridline') },
                gridline_7 = { constraint.typed_string('gridline') },
              })},
              mode_3 = { constraint.table_keys({
                gridline_1 = { constraint.typed_string('gridline') },
                gridline_2 = { constraint.typed_string('gridline') },
                gridline_3 = { constraint.typed_string('gridline') },
                gridline_4 = { constraint.typed_string('gridline') },
                gridline_5 = { constraint.typed_string('gridline') },
                gridline_6 = { constraint.typed_string('gridline') },
                gridline_7 = { constraint.typed_string('gridline') },
              })},
              mode_4 = { constraint.table_keys({
                gridline_1 = { constraint.typed_string('gridline') },
                gridline_2 = { constraint.typed_string('gridline') },
                gridline_3 = { constraint.typed_string('gridline') },
                gridline_4 = { constraint.typed_string('gridline') },
                gridline_5 = { constraint.typed_string('gridline') },
                gridline_6 = { constraint.typed_string('gridline') },
                gridline_7 = { constraint.typed_string('gridline') },
              })},
              mode_5 = { constraint.table_keys({
                gridline_1 = { constraint.typed_string('gridline') },
                gridline_2 = { constraint.typed_string('gridline') },
                gridline_3 = { constraint.typed_string('gridline') },
                gridline_4 = { constraint.typed_string('gridline') },
                gridline_5 = { constraint.typed_string('gridline') },
                gridline_6 = { constraint.typed_string('gridline') },
                gridline_7 = { constraint.typed_string('gridline') },
              })},
              mode_6 = { constraint.table_keys({
                gridline_1 = { constraint.typed_string('gridline') },
                gridline_2 = { constraint.typed_string('gridline') },
                gridline_3 = { constraint.typed_string('gridline') },
                gridline_4 = { constraint.typed_string('gridline') },
                gridline_5 = { constraint.typed_string('gridline') },
                gridline_6 = { constraint.typed_string('gridline') },
                gridline_7 = { constraint.typed_string('gridline') },
              })},
              mode_7 = { constraint.table_keys({
                gridline_1 = { constraint.typed_string('gridline') },
                gridline_2 = { constraint.typed_string('gridline') },
                gridline_3 = { constraint.typed_string('gridline') },
                gridline_4 = { constraint.typed_string('gridline') },
                gridline_5 = { constraint.typed_string('gridline') },
                gridline_6 = { constraint.typed_string('gridline') },
                gridline_7 = { constraint.typed_string('gridline') },
              })},
            })},
          }),
          constraint.table_keys({
            fresh_food_grid_deltas = { constraint.table_keys({
              fresh_food_no_freeze_limit_in_degfx100 = { constraint.typed_string('gridline') },
              fresh_food_low_hyst_in_degfx100 = { constraint.typed_string('gridline') },
              fresh_food_set_point_delta_in_degfx100 = { constraint.typed_string('gridline') },
              fresh_food_high_hyst_in_degfx100 = { constraint.typed_string('gridline') },
              fresh_food_extra_high_hyst_in_degfx100 = { constraint.typed_string('gridline') },
              fresh_food_super_high_hyst_in_degfx100 = { constraint.typed_string('gridline') }
            })},
            freezer_grid_deltas = { constraint.table_keys({
              freezer_low_hyst_in_degfx100 = { constraint.typed_string('gridline') },
              freezer_setpoint_delta_in_degfx100 = { constraint.typed_string('gridline') },
              freezer_high_hyst_in_degfx100 = { constraint.typed_string('gridline') },
              freezer_extra_high_hyst_in_degfx100 = { constraint.typed_string('gridline') },
              freezer_super_high_hyst_in_degfx100 = { constraint.typed_string('gridline') },
              freezer_extreme_high_hyst_in_degfx100 = { constraint.typed_string('gridline') }
            })}
          })
        })
      }
    })

    return generate(config)
  end
end
