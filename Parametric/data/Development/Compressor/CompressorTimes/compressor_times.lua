return compressor_times({
  minimum_off_time_in_minutes = 10,
  minimum_on_time_in_minutes = 15,
  startup_on_time_in_seconds = 100,
  remain_off_after_valve_move_in_minutes = 2,
  remain_off_after_valve_move_low_ambient_in_minutes = 3,
  variable_speed_minimum_run_time_in_minutes = 2,
  sabbath_delay_time_in_seconds = 20
})
