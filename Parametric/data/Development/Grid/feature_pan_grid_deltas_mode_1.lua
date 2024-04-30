local grid_line_temps = import("../Grid/feature_pan_grid_line_temperatures_mode_1.lua")

return {
  gridline_1 = gridline({
    delta_in_degfx100 = grid_line_temps.feature_pan_super_low_deadband_in_degfx100,
    correction = grid_line_correction.adjusted_setpoint,
    cross_ambient_hysteresis_adjustment = import('../Grid/cross_ambient_hysteresis_adjustment_disabled.lua')
  }),
  gridline_2 = gridline({
    delta_in_degfx100 = grid_line_temps.feature_pan_low_deadband_in_degfx100,
    correction = grid_line_correction.adjusted_setpoint,
    cross_ambient_hysteresis_adjustment = import('../Grid/cross_ambient_hysteresis_adjustment_disabled.lua')
  }),
  gridline_3 = gridline({
    delta_in_degfx100 = grid_line_temps.feature_pan_low_hyst_in_degfx100,
    correction = grid_line_correction.adjusted_setpoint,
    cross_ambient_hysteresis_adjustment = import('../Grid/cross_ambient_hysteresis_adjustment_disabled.lua')
  }),
  gridline_4 = gridline({
    delta_in_degfx100 = grid_line_temps.feature_pan_setpoint_delta_in_degfx100,
    correction = grid_line_correction.adjusted_setpoint,
    cross_ambient_hysteresis_adjustment = import('../Grid/cross_ambient_hysteresis_adjustment_disabled.lua')
  }),
  gridline_5 = gridline({
    delta_in_degfx100 = grid_line_temps.feature_pan_high_hyst_in_degfx100,
    correction = grid_line_correction.adjusted_setpoint,
    cross_ambient_hysteresis_adjustment = import('../Grid/cross_ambient_hysteresis_adjustment_disabled.lua')
  }),
  gridline_6 = gridline({
    delta_in_degfx100 = grid_line_temps.feature_pan_high_deadband_in_degfx100,
    correction = grid_line_correction.adjusted_setpoint,
    cross_ambient_hysteresis_adjustment = import('../Grid/cross_ambient_hysteresis_adjustment_disabled.lua')
  }),
  gridline_7 = gridline({
    delta_in_degfx100 = grid_line_temps.feature_pan_super_high_deadband_in_degfx100,
    correction = grid_line_correction.adjusted_setpoint,
    cross_ambient_hysteresis_adjustment = import('../Grid/cross_ambient_hysteresis_adjustment_disabled.lua')
  })
}
