return compressor_speeds({
  cooling_mode_dependent = false,
  super_low_speed_frequency_in_hz = 40,
  super_high_speed_frequency_in_hz = 50,
  startup_speed_frequency_in_hz = 20,
  delayed_convertible_compartment_speed_frequency_in_hz = 50,
  cooling_mode_independent_compressor_speeds = import('../CompressorSpeeds/cooling_mode_independent_compressor_speeds.lua')
})
