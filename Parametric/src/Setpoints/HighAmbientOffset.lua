local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
      return TypedString(
        { 'high_ambient_offset' },
        structure(
          u8(config.relative_humidity_trigger_for_high_ambient_humidity_offset_in_percent),
          i8(config.user_setpoint_trigger_for_high_ambient_humidity_offset_in_degf),
          i16(config.offset_for_high_ambient_humidity_in_degfx100)
        )
      )
  end)

  return function(config)
    validate_arguments(
      config,
      {
         relative_humidity_trigger_for_high_ambient_humidity_offset_in_percent = { constraint.in_range(0, 100) },
         user_setpoint_trigger_for_high_ambient_humidity_offset_in_degf = { constraint.i8 },
         offset_for_high_ambient_humidity_in_degfx100 = { constraint.i16 }
      }
    )
    return generate(config)
  end
end
