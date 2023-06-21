local import = require 'lua-common'.utilities.import
local memoize = require 'lua-common'.util.memoize
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString

return function(core)
   import(core)

   local generate = memoize(function(config)
    return TypedString(
      { 'fresh_food_adjusted_setpoint' },
      structure(
        i16(config.fresh_food_offset_in_degfx100),
        i16(config.fresh_food_pulldown_offset_in_degfx100),
        pointer(config.cross_ambient_offset),
        pointer(config.high_ambient_offset),
        pointer(config.setpoint_offset),
        pointer(config.shift_offset)
      )
    )
   end)

  return function(config)
    validate_arguments(
      config,
      {
        fresh_food_offset_in_degfx100 = { constraint.i16 },
        fresh_food_pulldown_offset_in_degfx100 = { constraint.i16 },
        cross_ambient_offset = { constraint.typed_string('cross_ambient_offset') },
        high_ambient_offset = { constraint.typed_string('high_ambient_offset') },
        setpoint_offset = { constraint.typed_string('setpoint_offset') },
        shift_offset = { constraint.typed_string('shift_offset') }
      })
    return generate(config)
  end
end
