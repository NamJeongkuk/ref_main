local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'fan_fault' },
      structure(
        u16(config.fan_missed_target_fault_timeout_in_seconds),
        u16(config.missing_fan_feedback_fault_timeout_in_seconds),
        u16(config.feedback_present_when_fan_is_off_timeout_in_seconds)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        fan_missed_target_fault_timeout_in_seconds = { constraint.u16 },
        missing_fan_feedback_fault_timeout_in_seconds = { constraint.u16 },
        feedback_present_when_fan_is_off_timeout_in_seconds = { constraint.u16 }
      })

    return generate(config)
  end
end
