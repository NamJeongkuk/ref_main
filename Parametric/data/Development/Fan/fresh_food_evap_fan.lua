return fan({
  fan_id = fan_id.fresh_food_evap,
  pulses_per_revolution = 2,
  care_about_cooling_mode = true,
  power_usage_in_watts = 8,
  pid = import("../Fan/Pid/pid.lua"),
  fault = import("../Fan/FanFault/fan_fault.lua"),
  speed_table = import("../Fan/SpeedTable/FreshFoodEvap/fan_care_about_cooling_mode_speed_table.lua"),
})
