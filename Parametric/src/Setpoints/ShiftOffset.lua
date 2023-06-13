local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
      return TypedString(
        { 'shift_offset' },
        structure(
          i16(config.max_shift_in_degfx100),
          i16(config.min_shift_in_degfx100),
          i16(config.upper_adjustment_limit_in_degfx100),
          i16(config.lower_adjustment_limit_in_degfx100),
          i16(config.post_defrost_shift_in_degfx100)
        )
      )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        max_shift_in_degfx100 = { constraint.i16 },
        min_shift_in_degfx100 = { constraint.i16 },
        upper_adjustment_limit_in_degfx100 = { constraint.i16 },
        lower_adjustment_limit_in_degfx100 = { constraint.i16 },
        post_defrost_shift_in_degfx100 = { constraint.in_range(config.min_shift_in_degfx100, config.max_shift_in_degfx100) }
      }
    )
    return generate(config)
  end
end
