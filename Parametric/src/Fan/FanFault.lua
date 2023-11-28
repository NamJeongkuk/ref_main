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
        u8(config.cannot_reach_target_fault_timeout_in_minutes),
        u8(config.missing_feedback_when_fan_on_fault_timeout_in_minutes),
        u8(config.feedback_present_when_fan_off_timeout_in_minutes)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        cannot_reach_target_fault_timeout_in_minutes = { constraint.u8 },
        missing_feedback_when_fan_on_fault_timeout_in_minutes = { constraint.u8 },
        feedback_present_when_fan_off_timeout_in_minutes = { constraint.u8 }
      })

    return generate(config)
  end
end
