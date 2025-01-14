return twist_tray_ice_maker({
  fill = {
    ice_maker_fill_monitor = import("../IceMaker/twist_tray_ice_maker_fill_monitor.lua")
  },
  freeze = {
    harvest_count_calculator = import("../IceMaker/twist_tray_harvest_count_calculator.lua"),
    maximum_harvest_temperature_in_deg_fx100 = 1900
  },
  harvest = {
    full_state_wait_period_in_minutes = 10,
    full_state_detection_period_in_secx10 = 11,
    initial_homing_twist_period_in_secx10 = 12,
    home_landing_delay_period_in_secx10 = 13,
    motor_error_retry_initialize_in_minutes = 14,
    long_motor_error_timeout_period_in_sec = 15,
    short_motor_error_timeout_period_in_sec = 16,
    delay_to_harvest_after_door_opens_in_minutes = 17,
    full_state_dispense_check_time_in_seconds = 18,
    freeze_thaw_fill_tube_heater_on_time_in_seconds = 400,
    freeze_thaw_fill_tube_heater_duty_cycle_percentage = 100,
    full_state_temperature_to_transition_to_freeze_state_in_degfx100 = 200,
    full_state_door_open_check_time_in_minutes = 5
  }
})
