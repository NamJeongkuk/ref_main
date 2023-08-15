local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'sabbath' },
      structure(
        u16(config.time_between_defrosts_in_minutes),
        u8(config.interior_lights_pwm_duty_cycle_percentage)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        time_between_defrosts_in_minutes = { constraint.u16 },
        interior_lights_pwm_duty_cycle_percentage = { constraint.u8 }
      })
    return generate(config)
  end
end
