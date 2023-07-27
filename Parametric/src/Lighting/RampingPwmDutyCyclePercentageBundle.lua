local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'ramping_pwm_duty_cycle_percentage_bundle' },
      structure(
        u8(config.max_pwm_duty_cycle_percentage),
        u8(config.min_pwm_duty_cycle_percentage),
        u16(config.ramping_up_to_max_count_per_msec),
        u16(config.ramping_down_to_off_count_per_msec)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        max_pwm_duty_cycle_percentage = { constraint.in_range(0,100) },
        min_pwm_duty_cycle_percentage = { constraint.in_range(0,100) },
        ramping_up_to_max_count_per_msec = { constraint.u16 },
        ramping_down_to_off_count_per_msec = { constraint.u16 }
      })
    return generate(config)
  end
end
