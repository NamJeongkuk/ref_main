local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'pid' },
      structure(
        structure(
          i32(config.kp_value),
          u8(config.kp_bits)
        ),
        structure(
          i32(config.ki_value),
          u8(config.ki_bits)
        ),
        structure(
          i32(config.kd_value),
          u8(config.kd_bits)
        ),
        i32(config.lower_limit),
        i32(config.upper_limit)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        kp_value = { constraint.i32 },
        kp_bits = { constraint.u8 },
        ki_value = { constraint.i32 },
        ki_bits = { constraint.u8 },
        kd_value = { constraint.i32 },
        kd_bits = { constraint.u8 },
        lower_limit = { constraint.i32 },
        upper_limit = { constraint.i32 },
      })

    return generate(config)
  end
end
