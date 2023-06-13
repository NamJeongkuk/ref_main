return compressor({
  compressor_is_single_speed = true,
  low_ambient_valve_trigger_temperature_in_deg_fx100 = 300,
  compressor_times = import('../Compressor/CompressorTimes/compressor_times.lua')
})
