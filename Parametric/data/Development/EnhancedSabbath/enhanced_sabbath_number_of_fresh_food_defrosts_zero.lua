return enhanced_sabbath({
   max_time_in_enhanced_sabbath_mode_in_minutes = 4 * constants.minutes_per_hour * constants.hours_per_day,
   fresh_food_setpoint_temperature_in_degfx100 = 3700,
   freezer_setpoint_temperature_in_degfx100 = 0,
   number_of_fresh_food_defrosts_before_freezer_defrost = 0,
   min_time_between_temperature_averaging_in_minutes = 0,
   fresh_food_stage_time_in_minutes = 5,
   freezer_stage_time_in_minutes = 3,
   off_stage_time_in_minutes = 2,
   interior_lights_pwm_duty_cycle_percentage = 20
})
