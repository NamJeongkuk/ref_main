local import = require 'lua-common'.utilities.import
local memoize = require 'lua-common'.util.memoize
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString

return function(core)
   import(core)

   local generate = memoize(function(config)
    return TypedString(
      { 'deli_pan_adjusted_setpoint' },
      structure(
        pointer(config.fixed_setpoint_offset),
        pointer(config.shift_offset)
      )
    )
   end)

  return function(config)
    validate_arguments(
      config,
      {
        fixed_setpoint_offset = { constraint.typed_string('fixed_setpoint_offset') },
        shift_offset = { constraint.typed_string('shift_offset') }
      })
    return generate(config)
  end
end
