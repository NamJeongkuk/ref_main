local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local fan_id = require 'Fan/FanId'
local Rpm = require 'Fan/Rpm'

return function(core)
  import(core)

  local rpm = Rpm(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'fan' },
      structure(
        bool(false),
        u8(config.fan_id),
        u8(config.pulses_per_revolution),
        bool(config.care_about_cooling_mode),
        u16(config.power_usage_in_watts),
        config.pid,
        config.fault,
        pointer(config.speed_table),
        rpm({ rpm = 0 }),
        rpm({ rpm = 0 }),
        i16(0),
        u16(0)
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
          speed_table = { constraint.typed_string('fan_care_about_cooling_mode_speed_table') }
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
          speed_table = { constraint.typed_string('fan_speed_table') }
        })
    end
    return generate(config)
  end
end
