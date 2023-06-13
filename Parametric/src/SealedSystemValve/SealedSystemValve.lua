local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'sealedSystemValve' },
      structure(
        u16(config.homing_time_in_ms)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        homing_time_in_ms = { constraint.u16 }
      })

    return generate(config)
  end
end
