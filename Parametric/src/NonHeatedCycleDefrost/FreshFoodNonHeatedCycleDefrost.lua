local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local fan_speed_type = require 'Fan.FanSpeedType'
local enum = require 'lua-common'.utilities.enum

return function(core)
  import(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'fresh_food_non_heated_cycle_defrost' },
      structure(
        bool(config.fan_speed_compressor_off_enable),
        bool(config.sealed_system_valve_change_enable),
        u8(config.default_time_valve_change_in_minutes),
        u8(config.sealed_system_valve_change_slope_in_minutesx10_per_degf),
        u8(config.max_time_valve_change_in_minutes),
        u8(config.min_time_valve_change_in_minutes),
        u8(fan_speed_type[config.fan_speed_compressor_off]),
        u8(fan_speed_type[config.fan_speed_valve_change])
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        fan_speed_compressor_off_enable = { constraint.boolean },
        sealed_system_valve_change_enable = { constraint.boolean },
        default_time_valve_change_in_minutes = { constraint.u8 },
        sealed_system_valve_change_slope_in_minutesx10_per_degf = { constraint.u8 },
        max_time_valve_change_in_minutes = { constraint.u8 },
        min_time_valve_change_in_minutes = { constraint.u8 },
        fan_speed_compressor_off = { constraint.in_set(enum.keys(fan_speed_type)) },
        fan_speed_valve_change = { constraint.in_set(enum.keys(fan_speed_type)) }
      })

    return generate(config)
  end
end
