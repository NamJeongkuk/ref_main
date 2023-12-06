return cabinet_temperature_exceeded({
  temperature_upper_hysteresis_limit_above_setpoint_in_degfx100 = 2000,
  temperature_lower_hysteresis_limit_above_setpoint_in_degfx100 = 1500,
  minimum_time_since_power_on_in_minutes = 8 * constants.minutes_per_hour,
  minimum_time_doors_must_be_closed_in_minutes = 60,
  minimum_time_since_defrost_has_completed_in_minutes = 60
})
