local import = require 'lua-common'.utilities.import
local memoize = require 'lua-common'.util.memoize
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString

return function(core)
  import(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'setpoint_zone' },
      structure(
        i16(config.freezer_trigger_cold_setpoint_zone_in_degfx100),
        i16(config.freezer_trigger_warm_setpoint_zone_in_degfx100),
        i16(config.fresh_food_trigger_cold_setpoint_zone_in_degfx100),
        i16(config.fresh_food_trigger_warm_setpoint_zone_in_degfx100),
        i16(config.convertible_compartment_trigger_setpoint_zone_in_degfx100)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        freezer_trigger_cold_setpoint_zone_in_degfx100 = { constraint.i16 },
        freezer_trigger_warm_setpoint_zone_in_degfx100 = { constraint.i16 },
        fresh_food_trigger_cold_setpoint_zone_in_degfx100 = { constraint.i16 },
        fresh_food_trigger_warm_setpoint_zone_in_degfx100 = { constraint.i16 },
        convertible_compartment_trigger_setpoint_zone_in_degfx100 = { constraint.i16 }
      })
    return generate(config)
  end
end
