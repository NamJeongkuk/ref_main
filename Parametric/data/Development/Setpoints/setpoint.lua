return setpoint({
  setpoint_zone = import('../Setpoints/setpoint_zone.lua'),
  adjusted_setpoint = import('../Setpoints/AdjustedSetpoint/adjusted_setpoint.lua'),
  user_setpoint_data = import('../Setpoints/user_setpoint_data.lua')
})
