local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'user_setpoint_data' },
      structure(
        pointer(config.fresh_food_user_setpoint),
        pointer(config.freezer_user_setpoint),
        pointer(config.feature_pan_user_setpoints)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        fresh_food_user_setpoint = { constraint.typed_string('user_setpoint') },
        freezer_user_setpoint = { constraint.typed_string('user_setpoint') },
        feature_pan_user_setpoints = { constraint.typed_string('feature_pan_user_setpoints') }
      })
    return generate(config)
  end
end
