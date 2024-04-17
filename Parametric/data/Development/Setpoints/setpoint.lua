return setpoint({
  setpoint_zone = import('../Setpoints/setpoint_zone.lua'),
  adjusted_setpoint = import('../Setpoints/AdjustedSetpoint/adjusted_setpoint_with_feature_pan_as_convertible_compartment.lua'),
  user_setpoint_data = import('../Setpoints/user_setpoint_data.lua')
})
