local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
      return TypedString(
        { 'setpoint_offset' },
        structure(
          i16(config.cold_setpoint_offset_in_degfx100),
          i16(config.mid_setpoint_offset_in_degfx100),
          i16(config.warm_setpoint_offset_in_degfx100)
        )
      )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        cold_setpoint_offset_in_degfx100 = { constraint.i16 },
        mid_setpoint_offset_in_degfx100 = { constraint.i16 },
        warm_setpoint_offset_in_degfx100 = { constraint.i16 }
      }
    )
    return generate(config)
  end
end
