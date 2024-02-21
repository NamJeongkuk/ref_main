local import = require 'lua-common'.utilities.import
local memoize = require 'lua-common'.util.memoize
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString

return function(core)
  import(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'adjusted_setpoint' },
      structure(
        pointer(config.fresh_food_adjusted_setpoint),
        pointer(config.freezer_adjusted_setpoint),
        pointer(config.convertible_compartment_adjusted_setpoint),
        pointer(config.deli_pan_adjusted_setpoint),
        pointer(config.ice_cabinet_adjusted_setpoint),
        pointer(config.shift_offset_calculator)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        fresh_food_adjusted_setpoint = { constraint.typed_string('fresh_food_adjusted_setpoint') },
        freezer_adjusted_setpoint = { constraint.typed_string('freezer_adjusted_setpoint') },
        convertible_compartment_adjusted_setpoint = { constraint.typed_string('convertible_compartment_adjusted_setpoint') },
        deli_pan_adjusted_setpoint = { constraint.typed_string('deli_pan_adjusted_setpoint') },
        ice_cabinet_adjusted_setpoint = { constraint.typed_string('ice_cabinet_adjusted_setpoint') },
        shift_offset_calculator = { constraint.typed_string('shift_offset_calculator') }
      })
    return generate(config)
  end
end
