local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local Rpm = require 'Fan/Rpm'

return function(core)
  import(core)
  local rpm = Rpm(core)
  local generate = memoize(function(config)
    if config.care_about_setpoint == true then
      return TypedString(
        { 'fan_care_about_cooling_mode_speed_table' },
        structure(
          bool(config.care_about_setpoint),
          config.super_low_speed,
          config.low_speed_fresh_food,
          config.low_speed_freezer_cold_setpoint,
          config.low_speed_freezer_medium_setpoint,
          config.low_speed_freezer_warm_setpoint,
          config.low_speed_convertible_compartment_cold_setpoint,
          config.low_speed_convertible_compartment_medium_setpoint,
          config.low_speed_convertible_compartment_warm_setpoint,
          config.medium_speed_fresh_food,
          config.medium_speed_freezer,
          config.medium_speed_convertible_compartment,
          config.high_speed_fresh_food,
          config.high_speed_freezer,
          config.high_speed_convertible_compartment,
          config.super_high_speed
        )
      )
    else
      return TypedString(
        { 'fan_care_about_cooling_mode_speed_table' },
        structure(
          bool(config.care_about_setpoint),
          config.super_low_speed,
          config.low_speed_fresh_food,
          config.low_speed_freezer,
          config.medium_speed_fresh_food,
          config.medium_speed_freezer,
          config.high_speed_fresh_food,
          config.high_speed_freezer,
          config.super_high_speed,
          rpm({rpm = 0}),
          rpm({rpm = 0}),
          rpm({rpm = 0}),
          rpm({rpm = 0}),
          rpm({rpm = 0}),
          rpm({rpm = 0}),
          rpm({rpm = 0})
        )
      )
    end
  end)

  return function(config)
    if config.care_about_setpoint == nil then
      validate_arguments(
        config,
        {
          care_about_setpoint = { constraint.boolean },
        })
    elseif config.care_about_setpoint == true then
      validate_arguments(
        config,
        {
          care_about_setpoint = { constraint.boolean },
          super_low_speed = { constraint.typed_string('speed') },
          low_speed_fresh_food = { constraint.typed_string('speed') },
          low_speed_freezer_cold_setpoint = { constraint.typed_string('speed') },
          low_speed_freezer_medium_setpoint = { constraint.typed_string('speed') },
          low_speed_freezer_warm_setpoint = { constraint.typed_string('speed') },
          low_speed_convertible_compartment_cold_setpoint = { constraint.typed_string('speed') },
          low_speed_convertible_compartment_medium_setpoint = { constraint.typed_string('speed') },
          low_speed_convertible_compartment_warm_setpoint = { constraint.typed_string('speed') },
          medium_speed_fresh_food = { constraint.typed_string('speed') },
          medium_speed_freezer = { constraint.typed_string('speed') },
          medium_speed_convertible_compartment = { constraint.typed_string('speed') },
          high_speed_fresh_food = { constraint.typed_string('speed') },
          high_speed_freezer = { constraint.typed_string('speed') },
          high_speed_convertible_compartment = { constraint.typed_string('speed') },
          super_high_speed = { constraint.typed_string('speed') }
        })
    else
      validate_arguments(
        config,
        {
          care_about_setpoint = { constraint.boolean },
          super_low_speed = { constraint.typed_string('speed') },
          low_speed_fresh_food = { constraint.typed_string('speed') },
          low_speed_freezer = { constraint.typed_string('speed') },
          medium_speed_fresh_food = { constraint.typed_string('speed') },
          medium_speed_freezer = { constraint.typed_string('speed') },
          high_speed_fresh_food = { constraint.typed_string('speed') },
          high_speed_freezer = { constraint.typed_string('speed') },
          super_high_speed = { constraint.typed_string('speed') }
        })
    end
    return generate(config)
  end
end
