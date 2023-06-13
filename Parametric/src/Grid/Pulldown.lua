local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'pulldown' },
      structure(
        bool(config.pulldown_in_medium_compressor_speed_enable),
        bool(config.compressor_trip_mitigation_enable)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        pulldown_in_medium_compressor_speed_enable = { constraint.boolean },
        compressor_trip_mitigation_enable = { constraint.boolean }
      })

    return generate(config)
  end
end
