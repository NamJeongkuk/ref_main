local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'cabinet_temperature_exceeded' },
      structure(
        i16(config.temperature_upper_hysteresis_limit_above_setpoint_in_degfx100),
        i16(config.temperature_lower_hysteresis_limit_above_setpoint_in_degfx100),
        u16(config.minimum_time_since_power_on_in_minutes),
        u8(config.minimum_time_doors_must_be_closed_in_minutes),
        u8(config.minimum_time_since_defrost_has_completed_in_minutes)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        temperature_upper_hysteresis_limit_above_setpoint_in_degfx100 = { constraint.i16 },
        temperature_lower_hysteresis_limit_above_setpoint_in_degfx100 = { constraint.i16, constraint.less_than(config.temperature_upper_hysteresis_limit_above_setpoint_in_degfx100) },
        minimum_time_since_power_on_in_minutes = { constraint.u16 },
        minimum_time_doors_must_be_closed_in_minutes = { constraint.u8 },
        minimum_time_since_defrost_has_completed_in_minutes = { constraint.u8 }
      })

    return generate(config)
  end
end
