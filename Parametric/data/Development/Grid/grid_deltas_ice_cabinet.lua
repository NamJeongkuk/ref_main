local grid_line_temps = import("../Grid/ice_cabinet_grid_line_temperatures.lua")

return grid_deltas({
  deltas = {
    grid_deltas = {
      mode_1 = {
        gridline_1 = gridline({
          delta_in_degfx100 = grid_line_temps.ice_cabinet_super_low_deadband_in_degfx100,
          correction = grid_line_correction.adjusted_setpoint,
          cross_ambient_hysteresis_adjustment = import('../Grid/cross_ambient_hysteresis_adjustment_disabled.lua')
        }),
        gridline_2 = gridline({
          delta_in_degfx100 = grid_line_temps.ice_cabinet_low_deadband_in_degfx100,
          correction = grid_line_correction.adjusted_setpoint,
          cross_ambient_hysteresis_adjustment = import('../Grid/cross_ambient_hysteresis_adjustment_disabled.lua')
        }),
        gridline_3 = gridline({
          delta_in_degfx100 = grid_line_temps.ice_cabinet_low_hyst_in_degfx100,
          correction = grid_line_correction.adjusted_setpoint,
          cross_ambient_hysteresis_adjustment = import('../Grid/cross_ambient_hysteresis_adjustment_disabled.lua')
        }),
        gridline_4 = gridline({
          delta_in_degfx100 = grid_line_temps.ice_cabinet_set_point_delta_in_degfx100,
          correction = grid_line_correction.adjusted_setpoint,
          cross_ambient_hysteresis_adjustment = import('../Grid/cross_ambient_hysteresis_adjustment_disabled.lua')
        }),
        gridline_5 = gridline({
          delta_in_degfx100 = grid_line_temps.ice_cabinet_high_hyst_in_degfx100,
          correction = grid_line_correction.adjusted_setpoint,
          cross_ambient_hysteresis_adjustment = import('../Grid/cross_ambient_hysteresis_adjustment_disabled.lua')
        }),
        gridline_6 = gridline({
          delta_in_degfx100 = grid_line_temps.ice_cabinet_high_deadband_in_degfx100,
          correction = grid_line_correction.adjusted_setpoint,
          cross_ambient_hysteresis_adjustment = import('../Grid/cross_ambient_hysteresis_adjustment_disabled.lua')
        }),
        gridline_7 = gridline({
          delta_in_degfx100 = grid_line_temps.ice_cabinet_super_high_deadband_in_degfx100,
          correction = grid_line_correction.adjusted_setpoint,
          cross_ambient_hysteresis_adjustment = import('../Grid/cross_ambient_hysteresis_adjustment_disabled.lua')
        })
      }
    }
  }
})
