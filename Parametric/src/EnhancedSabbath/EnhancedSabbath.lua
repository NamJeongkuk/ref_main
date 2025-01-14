local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'enhanced_sabbath' },
      structure(
        u16(config.max_time_in_enhanced_sabbath_mode_in_minutes),
        i16(config.fresh_food_setpoint_temperature_in_degfx100),
        i16(config.freezer_setpoint_temperature_in_degfx100),
        u8(config.number_of_secondary_only_defrosts_before_full_defrost),
        u8(config.min_time_between_temperature_averaging_in_minutes),
        u8(config.fresh_food_stage_time_in_minutes),
        u8(config.freezer_stage_time_in_minutes),
        u8(config.off_stage_time_in_minutes),
        i16(config.fresh_food_low_speed_hysteresis_in_degfx100),
        i16(config.fresh_food_medium_speed_hysteresis_in_degfx100),
        i16(config.freezer_low_speed_hysteresis_in_degfx100),
        i16(config.freezer_medium_speed_hysteresis_in_degfx100),
        u8(config.interior_lights_pwm_duty_cycle_percentage)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        max_time_in_enhanced_sabbath_mode_in_minutes = { constraint.u16 },
        fresh_food_setpoint_temperature_in_degfx100 = { constraint.i16 },
        freezer_setpoint_temperature_in_degfx100 = { constraint.i16 },
        number_of_secondary_only_defrosts_before_full_defrost = { constraint.u8 },
        min_time_between_temperature_averaging_in_minutes = { constraint.u8 },
        fresh_food_stage_time_in_minutes = { constraint.u8 },
        freezer_stage_time_in_minutes = { constraint.u8 },
        off_stage_time_in_minutes = { constraint.u8 },
        fresh_food_low_speed_hysteresis_in_degfx100 = { constraint.i16 },
        fresh_food_medium_speed_hysteresis_in_degfx100 = { constraint.i16 },
        freezer_low_speed_hysteresis_in_degfx100 = { constraint.i16 },
        freezer_medium_speed_hysteresis_in_degfx100 = { constraint.i16 },
        interior_lights_pwm_duty_cycle_percentage = { constraint.u8 }
      })

    return generate(config)
  end
end
