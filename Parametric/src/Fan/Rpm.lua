local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local fan_control_type = require 'Fan/FanControlType'

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'speed' },
      structure(
        u8(fan_control_type.rpm),
        u16(config.rpm)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        rpm = { constraint.u16 },
      })

    return generate(config)
  end
end
