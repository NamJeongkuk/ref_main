return fan_care_about_cooling_mode_speed_table({
  care_about_setpoint = true,
  super_low_speed = rpm({rpm = 800}),
  low_speed_fresh_food = rpm({rpm = 1050}),
  low_speed_freezer_cold_setpoint = rpm({rpm = 1150}),
  low_speed_freezer_medium_setpoint = rpm({rpm = 1050}),
  low_speed_freezer_warm_setpoint = rpm({rpm = 950}),
  low_speed_convertible_compartment_cold_setpoint = rpm({rpm = 1150}),
  low_speed_convertible_compartment_medium_setpoint = rpm({rpm = 1050}),
  low_speed_convertible_compartment_warm_setpoint = rpm({rpm = 950}),
  medium_speed_fresh_food = rpm({rpm = 1050}),
  medium_speed_freezer = rpm({rpm = 1050}),
  medium_speed_convertible_compartment = rpm({rpm = 1050}),
  high_speed_fresh_food = rpm({rpm = 1150}),
  high_speed_freezer = rpm({rpm = 1150}),
  high_speed_convertible_compartment = rpm({rpm = 1150}),
  super_high_speed = duty_cycle({duty_cycle = 100})
})
