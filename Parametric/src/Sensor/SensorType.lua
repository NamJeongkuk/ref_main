local import = require 'lua-common'.utilities.import
local memoize = require 'lua-common'.util.memoize
local map = require 'lua-common'.fp.map
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString

return function(core)
  import(core)

  local fill_table = function(table_to_fill, table_to_fill_with)
    for _, v in ipairs(table_to_fill_with) do
      table.insert(table_to_fill, v)
    end
  end

  local unpack_table = function(table)
    return structure(i32(table.raw), i32(table.mapped))
  end

  local generate_table = memoize(function(sensorTable)
    return pointer(
      structure(
        pointer(
          structure(
            table.unpack(map(sensorTable, unpack_table))
          )
        ),
        size(#sensorTable)
      )
    )
  end)

  local generate = memoize(function(config)
    local sensorTable = {}

    local invalid = config.mapping_table.invalid_value
    local beginningOfMappingsTable = {
      { raw = 0, mapped = invalid },
      { raw = config.mapping_table.data[1].raw - 1, mapped = invalid }
    }

    local endingOfMappingsTable = {
      { raw = config.mapping_table.data[#config.mapping_table.data].raw + 1, mapped = invalid },
      { raw = 65535, mapped = invalid }
    }

    fill_table(sensorTable, beginningOfMappingsTable)
    fill_table(sensorTable, config.mapping_table.data)
    fill_table(sensorTable, endingOfMappingsTable)

    return TypedString(
      { 'sensor_type' },
      structure(
        generate_table(sensorTable),
        i16(config.fallback_value_in_degfx100),
        u8(config.exponentially_moving_average_filter_alpha_numerator),
        u8(config.exponentially_moving_average_filter_alpha_denominator),
        u8(config.exponentially_moving_average_filter_window_size),
        structure(
          bool(config.slew_rate_filter_enabled),
          u16(config.slew_rate_filter_slew_rate_in_degfx100_per_second)
        ),
        u16(config.fallback_good_reading_counter_max),
        u16(config.fallback_bad_reading_counter_max),
        bool(config.enable_external_sensor_check)
      )
    )
  end)

  return function(config)
    validate_arguments(config, {
      mapping_table = {
        constraint.table_keys({
          data = {
            constraint.array_size({ constraint.positive }),
            constraint.array_elements({
              constraint.table_keys({
                raw = { constraint.in_range(0, 65535) },
                mapped = { constraint.in_range(-32768, 32767) }
              })
            })
          },
          invalid_value = { constraint.equal_to(30000) }
        })
      },
      fallback_value_in_degfx100 = { constraint.i16 },
      exponentially_moving_average_filter_alpha_numerator = { constraint.u8 },
      exponentially_moving_average_filter_alpha_denominator = { constraint.u8 },
      exponentially_moving_average_filter_window_size = { constraint.u8 },
      slew_rate_filter_slew_rate_in_degfx100_per_second = { constraint.u16 },
      slew_rate_filter_enabled = { constraint.bool },
      fallback_good_reading_counter_max = { constraint.u16 },
      fallback_bad_reading_counter_max = { constraint.u16 },
      enable_external_sensor_check = { constraint.bool }
    })

    return generate(config)
  end
end
