return compressor_times({
  minimum_off_time_in_minutes = 1,
  minimum_on_time_in_minutes = 1,
  startup_on_time_in_seconds = 15,
  remain_off_after_valve_move_in_minutes = 1,
  remain_off_after_valve_move_low_ambient_in_minutes = 2,
  variable_speed_minimum_run_time_in_minutes = 1,
  sabbath_delay_time_in_seconds = 15,
  compressor_excessive_off_time_to_trigger_fcode_in_minutes = 15,
  compressor_excessive_run_time_to_trigger_fcode_in_minutes = 30
})
