return condenser_fan({
  fan_id = fan_id.condenser,
  pulses_per_revolution = 4,
  care_about_cooling_mode = true,
  power_usage_in_watts = 8,
  pid = import("../Fan/Pid/pid.lua"),
  fault = import("../Fan/FanFault/fan_fault.lua"),
  speed_table = import("../Fan/SpeedTable/Condenser/fan_care_about_cooling_mode_speed_table.lua"),
  super_low_at_high_ambient_humidity_and_high_temperature = rpm({rpm = 1000}),
  super_low_at_high_ambient_humidity_and_low_temperature = rpm({rpm = 1025}),
  high_ambient_trigger_temperature_in_degfx100 = 8200,
  high_ambient_trigger_humidity_in_percentx100 = 7500
})
