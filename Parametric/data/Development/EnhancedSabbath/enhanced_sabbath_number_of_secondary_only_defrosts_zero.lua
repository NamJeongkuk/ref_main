return enhanced_sabbath({
  max_time_in_enhanced_sabbath_mode_in_minutes = 4 * constants.minutes_per_hour * constants.hours_per_day,
  fresh_food_setpoint_temperature_in_degfx100 = 3700,
  freezer_setpoint_temperature_in_degfx100 = 0,
  number_of_secondary_only_defrosts_before_full_defrost = 0,
  min_time_between_temperature_averaging_in_minutes = 0,
  fresh_food_stage_time_in_minutes = 5,
  freezer_stage_time_in_minutes = 3,
  off_stage_time_in_minutes = 2,
  fresh_food_low_speed_hysteresis_in_degfx100 = 200,
  fresh_food_medium_speed_hysteresis_in_degfx100 = 300,
  freezer_low_speed_hysteresis_in_degfx100 = 100,
  freezer_medium_speed_hysteresis_in_degfx100 = 200,
  interior_lights_pwm_duty_cycle_percentage = 20
})
