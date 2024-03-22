return fresh_food_non_heated_cycle_defrost({
  fan_speed_compressor_off_enable = false,
  sealed_system_valve_change_enable = false,
  default_time_valve_change_in_minutes = 10,
  sealed_system_valve_change_slope_in_minutesx10_per_degf = 20,
  max_time_valve_change_in_minutes = 30,
  min_time_valve_change_in_minutes = 1,
  fan_speed_compressor_off = 'low_speed',
  fan_speed_valve_change = 'medium_speed'
})
