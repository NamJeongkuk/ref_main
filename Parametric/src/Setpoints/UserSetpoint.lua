local import = require 'lua-common'.utilities.import
local memoize = require 'lua-common'.util.memoize
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString

return function(core)
   import(core)

   local generate = memoize(function(config)
    return TypedString(
      { 'user_setpoint' },
      structure(
        i8(config.min_temperature_setpoint_in_degf),
        i8(config.max_temperature_setpoint_in_degf),
        i8(config.default_temperature_setpoint_in_degf)
      )
    )
   end)

  return function(config)
    validate_arguments(
      config,
      {
        max_temperature_setpoint_in_degf = { constraint.i8 },
        min_temperature_setpoint_in_degf = { constraint.i8 },
        default_temperature_setpoint_in_degf = { constraint.in_range(config.min_temperature_setpoint_in_degf, config.max_temperature_setpoint_in_degf) }
      })
    return generate(config)
  end
end
