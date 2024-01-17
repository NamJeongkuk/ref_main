return twist_tray_ice_maker({
  fill = {
    ice_maker_fill_monitor = import("../IceMaker/twist_tray_ice_maker_fill_monitor_qa.lua")
  },
  freeze = {
    harvest_count_calculator = import("../IceMaker/twist_tray_harvest_count_calculator_qa.lua"),
    maximum_harvest_temperature_in_deg_fx100 = 1900,
  },
  harvest = {
    full_state_wait_period_in_minutes = 10,
    full_state_detection_period_in_secx10 = 70,
    initial_homing_twist_period_in_secx10 = 30,
    home_landing_delay_period_in_secx10 = 5,
    motor_error_retry_initialize_in_minutes = 10,
    long_motor_error_timeout_period_in_sec = 25,
    short_motor_error_timeout_period_in_sec = 3,
    delay_to_harvest_after_door_opens_in_minutes = 5,
    full_state_dispense_check_time_in_seconds = 10,
    freeze_thaw_fill_tube_heater_on_time_in_seconds = 30,
    freeze_thaw_fill_tube_heater_duty_cycle_percentage = 50,
    full_state_temperature_to_transition_to_freeze_state_in_degfx100 = 200,
    full_state_door_open_check_time_in_minutes = 5,
  }
})
