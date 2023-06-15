local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'variable_sweat_heater' },
      structure(
        u8(config.fallback_duty_cycle_in_percent),
        u8(config.heater_control_type),
        i32(config.temperature_coefficient),
        i32(config.humidity_coefficient),
        i32(config.fresh_food_coefficient),
        i32(config.freezer_coefficient),
        i16(config.temperature_squared_coefficient),
        i16(config.humidity_squared_coefficient),
        i16(config.fresh_food_squared_coefficient),
        i16(config.freezer_squared_coefficient),
        i16(config.temperature_humidity_coefficient),
        i16(config.temperature_fresh_food_coefficient),
        i16(config.temperature_freezer_coefficient),
        i16(config.humidity_fresh_food_coefficient),
        i16(config.humidity_freezer_coefficient),
        i16(config.fresh_food_freezer_coefficient),
        i32(config.intercept_coefficient)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        fallback_duty_cycle_in_percent = { constraint.in_range(0,100) },
        heater_control_type = { constraint.in_range(1,7) },
        temperature_coefficient = { constraint.i32 },
        humidity_coefficient = { constraint.i32 },
        fresh_food_coefficient = { constraint.i32 },
        freezer_coefficient = { constraint.i32 },
        temperature_squared_coefficient = { constraint.i16 },
        humidity_squared_coefficient = { constraint.i16 },
        fresh_food_squared_coefficient = { constraint.i16 },
        freezer_squared_coefficient = { constraint.i16 },
        temperature_humidity_coefficient = { constraint.i16 },
        temperature_fresh_food_coefficient = { constraint.i16 },
        temperature_freezer_coefficient = { constraint.i16 },
        humidity_fresh_food_coefficient = { constraint.i16 },
        humidity_freezer_coefficient = { constraint.i16 },
        fresh_food_freezer_coefficient = { constraint.i16 },
        intercept_coefficient = { constraint.i32 }
      })

    return generate(config)
  end
end
