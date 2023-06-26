local import = require 'lua-common'.utilities.import
local memoize = require 'lua-common'.util.memoize
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString

return function(core)
  import(core)

  local generate = memoize(function(config)
  return TypedString(
    { 'fresh_food_turbo_cool_setpoint' },
      structure(
        i16(config.turbo_cool_setpoint_in_degfx100),
        u16(config.turbo_cool_on_time_in_minutes)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        turbo_cool_setpoint_in_degfx100 = { constraint.i16 },
        turbo_cool_on_time_in_minutes = { constraint.u16 }
      })
    return generate(config)
  end
end