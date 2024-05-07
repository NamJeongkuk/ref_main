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
      { 'ice_cabinet_fan_startup' },
      structure(
        u16(fan_speed_type[config.startup_speed]),
        u8(config.startup_time_in_seconds)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        startup_speed = { constraint.in_set(enum.keys(fan_speed_type)) },
        startup_time_in_seconds = { constraint.u8 }
      })

    return generate(config)
  end
end
