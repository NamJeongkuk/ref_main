local import = require 'lua-common'.utilities.import
local memoize = require 'lua-common'.util.memoize
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString

return function(core)
   import(core)

   local generate = memoize(function(config)
    return TypedString(
      { 'ice_box_adjusted_setpoint' },
      structure(
        i16(config.ice_formation_offset_in_degfx100),
        pointer(config.shift_offset)
      )
    )
   end)

  return function(config)
    validate_arguments(
      config,
      {
        ice_formation_offset_in_degfx100 = { constraint.i16 },
        shift_offset = { constraint.typed_string('shift_offset') }
      })
    return generate(config)
  end
end
