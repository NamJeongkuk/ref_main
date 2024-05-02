return feature_pan({
  feature_pan_warmup_slope_lower_limit_in_degfx100_per_minute = 100,
  feature_pan_warmup_slope_upper_limit_in_degfx100_per_minute = 200,
  feature_pan_heater_high_ambient_temperature_limit_degFx100 = 8000,
  feature_pan_pulldown_temperature_limit_in_degfx100 = 5000,
  fresh_food_fan_deli_cooling_speed = 'low_speed',
  feature_pan_default_mode = feature_pan_mode.mode1,
  feature_pan_min_allowed_mode = feature_pan_mode.mode1,
  feature_pan_max_allowed_mode = feature_pan_mode.mode5
})
