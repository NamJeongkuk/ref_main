local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
      return TypedString(
        { 'cooling_mode_dependent_compressor_speeds' },
        structure(
          u8(config.low_speed_fresh_food_warm_frequency_in_hz),
          u8(config.low_speed_fresh_food_mid_frequency_in_hz),
          u8(config.low_speed_fresh_food_cold_frequency_in_hz),
          u8(config.low_speed_freezer_warm_frequency_in_hz),
          u8(config.low_speed_freezer_mid_frequency_in_hz),
          u8(config.low_speed_freezer_cold_frequency_in_hz),
          u8(config.low_speed_convertible_compartment_warm_frequency_in_hz),
          u8(config.low_speed_convertible_compartment_mid_frequency_in_hz),
          u8(config.low_speed_convertible_compartment_cold_frequency_in_hz),
          u8(config.medium_speed_fresh_food_warm_frequency_in_hz),
          u8(config.medium_speed_fresh_food_mid_frequency_in_hz),
          u8(config.medium_speed_fresh_food_cold_frequency_in_hz),
          u8(config.medium_speed_freezer_warm_frequency_in_hz),
          u8(config.medium_speed_freezer_mid_frequency_in_hz),
          u8(config.medium_speed_freezer_cold_frequency_in_hz),
          u8(config.medium_speed_convertible_compartment_warm_frequency_in_hz),
          u8(config.medium_speed_convertible_compartment_mid_frequency_in_hz),
          u8(config.medium_speed_convertible_compartment_cold_frequency_in_hz),
          u8(config.high_speed_fresh_food_frequency_in_hz),
          u8(config.high_speed_freezer_frequency_in_hz),
          u8(config.high_speed_convertible_compartment_frequency_in_hz)
        )
      )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        low_speed_fresh_food_warm_frequency_in_hz = { constraint.u8 },
        low_speed_fresh_food_mid_frequency_in_hz = { constraint.u8 },
        low_speed_fresh_food_cold_frequency_in_hz = { constraint.u8 },
        low_speed_freezer_warm_frequency_in_hz = { constraint.u8 },
        low_speed_freezer_mid_frequency_in_hz = { constraint.u8 },
        low_speed_freezer_cold_frequency_in_hz = { constraint.u8 },
        low_speed_convertible_compartment_warm_frequency_in_hz = { constraint.u8 },
        low_speed_convertible_compartment_mid_frequency_in_hz = { constraint.u8 },
        low_speed_convertible_compartment_cold_frequency_in_hz = { constraint.u8 },
        medium_speed_fresh_food_warm_frequency_in_hz = { constraint.u8 },
        medium_speed_fresh_food_mid_frequency_in_hz = { constraint.u8 },
        medium_speed_fresh_food_cold_frequency_in_hz = { constraint.u8 },
        medium_speed_freezer_warm_frequency_in_hz = { constraint.u8 },
        medium_speed_freezer_mid_frequency_in_hz = { constraint.u8 },
        medium_speed_freezer_cold_frequency_in_hz = { constraint.u8 },
        medium_speed_convertible_compartment_warm_frequency_in_hz = { constraint.u8 },
        medium_speed_convertible_compartment_mid_frequency_in_hz = { constraint.u8 },
        medium_speed_convertible_compartment_cold_frequency_in_hz = { constraint.u8 },
        high_speed_fresh_food_frequency_in_hz = { constraint.u8 },
        high_speed_freezer_frequency_in_hz = { constraint.u8 },
        high_speed_convertible_compartment_frequency_in_hz = { constraint.u8 },
      }
    )
    return generate(config)
  end
end
