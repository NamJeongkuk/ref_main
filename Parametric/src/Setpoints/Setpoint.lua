local import = require 'lua-common'.utilities.import
local memoize = require 'lua-common'.util.memoize
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString

return function(core)
   import(core)

   local generate = memoize(function(config)
    return TypedString(
      { 'setpoint' },
      structure(
        pointer(config.setpoint_zone),
        pointer(config.adjusted_setpoint),
        pointer(config.user_setpoint_data)
      )
    )
   end)

  return function(config)
    validate_arguments(
      config,
      {
        setpoint_zone = { constraint.typed_string('setpoint_zone') },
        adjusted_setpoint = { constraint.typed_string('adjusted_setpoint') },
        user_setpoint_data = { constraint.typed_string('user_setpoint_data') }
      })
    return generate(config)
  end
end
