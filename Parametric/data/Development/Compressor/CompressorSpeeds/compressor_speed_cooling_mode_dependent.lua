return compressor_speeds({
  cooling_mode_dependent = true,
  super_low_speed_frequency_in_hz = 40,
  super_high_speed_frequency_in_hz = 50,
  startup_speed_frequency_in_hz = 30,
  cooling_mode_dependent_compressor_speeds = import('../CompressorSpeeds/cooling_mode_dependent_compressor_speeds.lua')
})
