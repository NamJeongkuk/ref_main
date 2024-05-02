return compressor_times({
  minimum_off_time_in_minutes = 5,
  minimum_on_time_in_minutes = 5,
  startup_on_time_in_seconds = 100,
  remain_off_after_valve_move_in_minutes = 2,
  remain_off_after_valve_move_low_ambient_in_minutes = 3,
  sabbath_delay_time_in_seconds = 20,
  compressor_excessive_off_time_to_trigger_fcode_in_minutes = 180,
  compressor_excessive_run_time_to_trigger_fcode_in_minutes = 1440
})
