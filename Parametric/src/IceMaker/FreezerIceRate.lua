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
    if config.time_in_minutes > 0 then
      return TypedString(
        { 'freezer_ice_rate' },
        structure(
          u8(config.time_in_minutes),
          i16(config.freezer_setpoint_in_degfx100),
          u8(fan_speed_type[config.freezer_evaporator_fan_speed])
        )
      )
    else
      return TypedString(
        { 'freezer_ice_rate' },
        structure(
          u8(config.time_in_minutes),
          i16(0),
          u8(0)
        )
      )
    end
  end)

  return function(config)
    validate_arguments(
      config,
      {
        time_in_minutes = { constraint.u8 },
      },
      {
        freezer_setpoint_in_degfx100 = { constraint.i16 },
        freezer_evaporator_fan_speed = { constraint.in_set(enum.keys(fan_speed_type)) }
      }
    )
    if config.time_in_minutes > 0 then
      validate_arguments(
        config,
        {
          time_in_minutes = { constraint.u8 },
          freezer_setpoint_in_degfx100 = { constraint.i16 },
          freezer_evaporator_fan_speed = { constraint.in_set(enum.keys(fan_speed_type)) }
        }
      )
    end
    return generate(config)
  end
end
