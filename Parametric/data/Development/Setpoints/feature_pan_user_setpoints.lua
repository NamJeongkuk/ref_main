return feature_pan_user_setpoints({
  setpoints = {
    feature_pan_user_setpoint({
      max_temperature_setpoint_in_degfx100 = 4001,
      min_temperature_setpoint_in_degfx100 = 3001,
      default_temperature_setpoint_in_degfx100 = 3401,
      mode_label = feature_pan_mode_label.refrigerator
    }),
    feature_pan_user_setpoint({
      max_temperature_setpoint_in_degfx100 = 4002,
      min_temperature_setpoint_in_degfx100 = 3002,
      default_temperature_setpoint_in_degfx100 = 3402,
      mode_label = feature_pan_mode_label.chiller
    }),
    feature_pan_user_setpoint({
      max_temperature_setpoint_in_degfx100 = 4003,
      min_temperature_setpoint_in_degfx100 = 3003,
      default_temperature_setpoint_in_degfx100 = 3403,
      mode_label = feature_pan_mode_label.soft_freeze
    }),
    feature_pan_user_setpoint({
      max_temperature_setpoint_in_degfx100 = 4004,
      min_temperature_setpoint_in_degfx100 = 3004,
      default_temperature_setpoint_in_degfx100 = 3404,
      mode_label = feature_pan_mode_label.meat
    }),
    feature_pan_user_setpoint({
      max_temperature_setpoint_in_degfx100 = 4005,
      min_temperature_setpoint_in_degfx100 = 3005,
      default_temperature_setpoint_in_degfx100 = 3405,
      mode_label = feature_pan_mode_label.beverages
    }),
    feature_pan_user_setpoint({
      max_temperature_setpoint_in_degfx100 = 4006,
      min_temperature_setpoint_in_degfx100 = 3006,
      default_temperature_setpoint_in_degfx100 = 3406,
      mode_label = feature_pan_mode_label.snacks
    }),
    feature_pan_user_setpoint({
      max_temperature_setpoint_in_degfx100 = 4007,
      min_temperature_setpoint_in_degfx100 = 3007,
      default_temperature_setpoint_in_degfx100 = 3407,
      mode_label = feature_pan_mode_label.wine
    })
  }
})
