local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
      return TypedString(
        { 'fixed_setpoint_offset' },
        structure(
          i16(config.mode1_offset_in_degfx100),
          i16(config.mode2_offset_in_degfx100),
          i16(config.mode3_offset_in_degfx100),
          i16(config.mode4_offset_in_degfx100),
          i16(config.mode5_offset_in_degfx100),
          i16(config.mode6_offset_in_degfx100),
          i16(config.mode7_offset_in_degfx100)
        )
      )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        mode1_offset_in_degfx100 = { constraint.i16 },
        mode2_offset_in_degfx100 = { constraint.i16 },
        mode3_offset_in_degfx100 = { constraint.i16 },
        mode4_offset_in_degfx100 = { constraint.i16 },
        mode5_offset_in_degfx100 = { constraint.i16 },
        mode6_offset_in_degfx100 = { constraint.i16 },
        mode7_offset_in_degfx100 = { constraint.i16 }
      }
    )
    return generate(config)
  end
end
