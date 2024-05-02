local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

local number_of_supported_feature_pan_modes = 7

return function(core)
  import(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'feature_pan_user_setpoints' },
      structure(
        pointer(structure(table.unpack(config.setpoints)))
      )
    )
  end)

  return function(config)
    validate_arguments(config, {
      setpoints = {
        constraint.array_size({ constraint.equal_to(number_of_supported_feature_pan_modes) }),
        constraint.array_elements({ constraint.typed_string('feature_pan_user_setpoint') })
      }
    })

    return generate(config)
  end
end
