local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'cross_ambient_hysteresis_adjustment' },
      structure(
        i16(config.multiplier),
        i16(config.divider)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        multiplier = { constraint.i16 },
        divider = { constraint.i16, constraint.not_equal_to(0) }
      }
    )
    return generate(config)
  end
end
