local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'door_lighting' },
      structure(
        config.normal_operation_ramping_pwm_duty_cycle_percentage_bundle
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        normal_operation_ramping_pwm_duty_cycle_percentage_bundle = { constraint.typed_string('ramping_pwm_duty_cycle_percentage_bundle') }
      })
    return generate(config)
  end
end
