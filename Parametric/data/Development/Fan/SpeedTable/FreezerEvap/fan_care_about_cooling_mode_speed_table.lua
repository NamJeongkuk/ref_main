return fan_care_about_cooling_mode_speed_table({
  care_about_freezer_setpoint = true,
  super_low_speed = rpm({rpm = 1300}),
  low_speed_fresh_food = rpm({rpm = 1500}),
  low_speed_freezer_cold_setpoint = rpm({rpm = 1800}),
  low_speed_freezer_medium_setpoint = rpm({rpm = 1700}),
  low_speed_freezer_warm_setpoint = rpm({rpm = 1600}),
  low_speed_convertible_compartment_cold_setpoint = rpm({rpm = 1800}),
  low_speed_convertible_compartment_medium_setpoint = rpm({rpm = 1700}),
  low_speed_convertible_compartment_warm_setpoint = rpm({rpm = 1600}),
  medium_speed_fresh_food = rpm({rpm = 1600}),
  medium_speed_freezer = rpm({rpm = 1800}),
  medium_speed_convertible_compartment = rpm({rpm = 1800}),
  high_speed_fresh_food = rpm({rpm = 1800}),
  high_speed_freezer = rpm({rpm = 2000}),
  high_speed_convertible_compartment = rpm({rpm = 2000}),
  super_high_speed = rpm({rpm = 0})
})
