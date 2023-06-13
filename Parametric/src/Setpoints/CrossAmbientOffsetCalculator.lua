local import = require 'lua-common'.utilities.import
local memoize = require 'lua-common'.util.memoize
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString

return function(core)
   import(core)

   local generate = memoize(function(config)
    return TypedString(
      { 'cross_ambient_offset_calculator' },
      structure(
        u8(config.ambient_temperature_average_window_size),
        u8(config.ambient_temperature_average_update_interval_in_minutes)
      )
    )
   end)

  return function(config)
    validate_arguments(
      config,
      {
        ambient_temperature_average_window_size = { constraint.u8 },
        ambient_temperature_average_update_interval_in_minutes = { constraint.u8 }
      })
    return generate(config)
  end
end
