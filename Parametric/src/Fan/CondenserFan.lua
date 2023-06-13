local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local fan_id = require 'Fan/FanId'

return function(core)
  import(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'condenser_fan' },
      structure(
        bool(true),
        u8(config.fan_id),
        u8(config.pulses_per_revolution),
        bool(config.care_about_cooling_mode),
        u16(config.power_usage_in_watts),
        config.pid,
        config.fault,
        pointer(config.speed_table),
        config.super_low_at_high_ambient_humidity_and_high_temperature,
        config.super_low_at_high_ambient_humidity_and_low_temperature,
        i16(config.high_ambient_trigger_temperature_in_degfx100),
        u16(config.high_ambient_trigger_humidity_in_percentx100)
      )
    )
  end)

  return function(config)
    if config.care_about_cooling_mode == nil then
      validate_arguments(
        config,
        {
          care_about_cooling_mode = { constraint.boolean }
        })
    elseif config.care_about_cooling_mode == true then
      validate_arguments(
        config,
        {
          fan_id = { constraint.in_enumeration(fan_id) },
          care_about_cooling_mode = { constraint.boolean },
          pulses_per_revolution = { constraint.u8 },
          power_usage_in_watts = { constraint.u16 },
          pid = { constraint.typed_string('pid') },
          fault = { constraint.typed_string('fan_fault') },
          speed_table = { constraint.typed_string('fan_care_about_cooling_mode_speed_table') },
          super_low_at_high_ambient_humidity_and_high_temperature = { constraint.typed_string('speed') },
          super_low_at_high_ambient_humidity_and_low_temperature = { constraint.typed_string('speed') },
          high_ambient_trigger_temperature_in_degfx100 = { constraint.i16 },
          high_ambient_trigger_humidity_in_percentx100 = { constraint.u16 }
        })
    else
      validate_arguments(
        config,
        {
          fan_id = { constraint.in_enumeration(fan_id) },
          care_about_cooling_mode = { constraint.boolean },
          pulses_per_revolution = { constraint.u8 },
          power_usage_in_watts = { constraint.u16 },
          pid = { constraint.typed_string('pid') },
          fault = { constraint.typed_string('fan_fault') },
          speed_table = { constraint.typed_string('fan_speed_table') },
          super_low_at_high_ambient_humidity_and_high_temperature = { constraint.typed_string('speed') },
          super_low_at_high_ambient_humidity_and_low_temperature = { constraint.typed_string('speed') },
          high_ambient_trigger_temperature_in_degfx100 = { constraint.i16 },
          high_ambient_trigger_humidity_in_percentx100 = { constraint.u16 }
        })
    end
    return generate(config)
  end
end
