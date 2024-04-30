return feature_pan_user_setpoints({
  setpoints = {
    feature_pan_user_setpoint({
      max_temperature_setpoint_in_degfx100 = 3000,
      min_temperature_setpoint_in_degfx100 = 1600,
      default_temperature_setpoint_in_degfx100 = 2300,
      mode_label = feature_pan_mode_label.refrigerator
    }),
    feature_pan_user_setpoint({
      max_temperature_setpoint_in_degfx100 = 3600,
      min_temperature_setpoint_in_degfx100 = 2200,
      default_temperature_setpoint_in_degfx100 = 2900,
      mode_label = feature_pan_mode_label.chiller
    }),
    feature_pan_user_setpoint({
      max_temperature_setpoint_in_degfx100 = 4000,
      min_temperature_setpoint_in_degfx100 = 2600,
      default_temperature_setpoint_in_degfx100 = 3300,
      mode_label = feature_pan_mode_label.soft_freeze
    }),
    feature_pan_user_setpoint({
      max_temperature_setpoint_in_degfx100 = 4200,
      min_temperature_setpoint_in_degfx100 = 3000,
      default_temperature_setpoint_in_degfx100 = 3700,
      mode_label = feature_pan_mode_label.meat
    }),
    feature_pan_user_setpoint({
      max_temperature_setpoint_in_degfx100 = 4900,
      min_temperature_setpoint_in_degfx100 = 3500,
      default_temperature_setpoint_in_degfx100 = 4200,
      mode_label = feature_pan_mode_label.beverages
    }),
    feature_pan_user_setpoint({
      max_temperature_setpoint_in_degfx100 = 3000,
      min_temperature_setpoint_in_degfx100 = 1600,
      default_temperature_setpoint_in_degfx100 = 2300,
      mode_label = feature_pan_mode_label.refrigerator
    }),
    feature_pan_user_setpoint({
      max_temperature_setpoint_in_degfx100 = 3000,
      min_temperature_setpoint_in_degfx100 = 1600,
      default_temperature_setpoint_in_degfx100 = 2300,
      mode_label = feature_pan_mode_label.refrigerator
    })
  }
})
