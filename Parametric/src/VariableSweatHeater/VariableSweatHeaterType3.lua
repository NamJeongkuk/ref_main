local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'variable_sweat_heater_type_3' },
      structure(
        i32(config.temperature_coefficient),
        i32(config.fresh_food_coefficient),
        i16(config.temperature_squared_coefficient),
        i16(config.fresh_food_squared_coefficient),
        i16(config.temperature_fresh_food_coefficient),
        i32(config.intercept_coefficient)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        temperature_coefficient = { constraint.i32 },
        fresh_food_coefficient = { constraint.i32 },
        temperature_squared_coefficient = { constraint.i16 },
        fresh_food_squared_coefficient = { constraint.i16 },
        temperature_fresh_food_coefficient = { constraint.i16 },
        intercept_coefficient = { constraint.i32 },
      })

    return generate(config)
  end
end
