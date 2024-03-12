return compressor({
  compressor_is_single_speed = false,
  low_ambient_valve_trigger_temperature_in_deg_fx100 = 300,
  variable_speed_compressor_relay_is_enabled = false,
  compressor_times = import('../Compressor/CompressorTimes/compressor_times_qa.lua'),
  compressor_speeds = import('../Compressor/CompressorSpeeds/compressor_speed_cooling_mode_dependent.lua')
})
