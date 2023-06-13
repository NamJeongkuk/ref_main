return single_damper({
  steps_to_open = 650,
  steps_to_close = 700,
  steps_to_home = 1850,
  delay_between_step_events_in_ms = 5,
  max_time_for_damper_to_be_open_before_cycling_in_minutes = 11,
  target_compartment_minimum_temperature_change_time_in_minutes = 3,
  target_compartment_damper_heater_on_time_in_minutes = 2,
  source_compartment_maximum_temperature_to_run_check_in_degfx100 = 500,
  target_compartment_minimum_temperature_change_in_degfx100 = 10
})
