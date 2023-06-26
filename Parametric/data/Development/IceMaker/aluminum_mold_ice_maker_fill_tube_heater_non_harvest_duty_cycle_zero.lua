return aluminum_mold_ice_maker({
  information = {
    type = 'aluminum_mold',
    location = 'freezer'
  },
  fill = {
    ice_maker_fill_monitor = import("../IceMaker/aluminum_mold_ice_maker_fill_monitor.lua")
  },
  freeze = {
    maximum_harvest_temperature_in_deg_fx100 = 1900,
    start_integration_temperature_in_deg_fx100 = 3200,
    freeze_integration_limit_in_deg_fx100_times_seconds = 3000000,
    minimum_freeze_time_in_minutes = 50,
    minimum_feeler_arm_extension_time_in_minutes = 3,
  },
  harvest = {
    maximum_harvest_time_in_minutes = 7,
    initial_motor_on_temperature_in_deg_fx100 = 3560,
    initial_minimum_heater_on_time_in_seconds = 30,
    heater_off_temperature_in_deg_fx100 = 5000,
    heater_on_temperature_in_deg_fx100 = 3200,
    rake_not_home_test_time_in_seconds = 10,
    feeler_arm_test_time_in_seconds = 10
  },
  harvest_fault = {
    rake_motor_control_time_in_seconds = 30,
    harvest_fault_max_time_in_minutes = 360
  },
  harvest_fix = {
    heater_off_temperature_in_deg_fx100 = 7800,
    heater_on_temperature_in_deg_fx100 = 6900,
    motor_off_time_in_seconds = 30,
    motor_on_time_in_seconds = 30,
    maximum_harvest_fix_time_in_minutes = 60
  },
  fill_tube_heater = {
    freeze_thaw_fill_tube_heater_duty_cycle_percentage = 100,
    freeze_thaw_fill_tube_heater_on_time_in_seconds = 400,
    non_harvest_fill_tube_heater_duty_cycle_percentage = 0
  }
})