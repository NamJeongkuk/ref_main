return fan({
  fan_id = fan_id.convertible_compartment,
  pulses_per_revolution = 2,
  care_about_cooling_mode = false,
  power_usage_in_watts = 8,
  pid = import("../Fan/Pid/pid.lua"),
  fault = import("../Fan/FanFault/fan_fault.lua"),
  speed_table = import("../Fan/SpeedTable/ConvertibleCompartment/fan_speed_table.lua"),
})
