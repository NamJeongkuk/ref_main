local grid_line_temps = import("../Grid/feature_pan_grid_line_temperatures.lua")

return grid_deltas({
  deltas = {
    grid_deltas = {
      gridline_1 = gridline({
        delta_in_degfx100 = grid_line_temps.feature_pan_super_low_deadband_in_degfx100,
        correction = 'adjusted_setpoint',
        cross_ambient_hysteresis_adjustment = cross_ambient_hysteresis_adjustment({
          multiplier = 1,
          divider = 1
        })
      }),
      gridline_2 = gridline({
        delta_in_degfx100 = grid_line_temps.feature_pan_low_deadband_in_degfx100,
        correction = 'adjusted_setpoint',
        cross_ambient_hysteresis_adjustment = cross_ambient_hysteresis_adjustment({
          multiplier = 1,
          divider = 1
        })
      }),
      gridline_3 = gridline({
        delta_in_degfx100 = grid_line_temps.feature_pan_low_hyst_in_degfx100,
        correction = 'adjusted_setpoint',
        cross_ambient_hysteresis_adjustment = cross_ambient_hysteresis_adjustment({
          multiplier = 1,
          divider = 1
        })
      }),
      gridline_4 = gridline({
        delta_in_degfx100 = grid_line_temps.feature_pan_high_hyst_in_degfx100,
        correction = 'adjusted_setpoint',
        cross_ambient_hysteresis_adjustment = cross_ambient_hysteresis_adjustment({
          multiplier = -1,
          divider = 1
        })
      }),
      gridline_5 = gridline({
        delta_in_degfx100 = grid_line_temps.feature_pan_high_deadband_in_degfx100,
        correction = 'adjusted_setpoint',
        cross_ambient_hysteresis_adjustment = cross_ambient_hysteresis_adjustment({
          multiplier = -1,
          divider = 1
        })
      }),
      gridline_6 = gridline({
        delta_in_degfx100 = grid_line_temps.feature_pan_setpoint_offset_in_degfx100,
        correction = 'adjusted_setpoint',
        cross_ambient_hysteresis_adjustment = cross_ambient_hysteresis_adjustment({
          multiplier = -1,
          divider = 1
        })
      }),
      gridline_7 = gridline({
        delta_in_degfx100 = grid_line_temps.feature_pan_super_high_deadband_in_degfx100,
        correction = 'adjusted_setpoint',
        cross_ambient_hysteresis_adjustment = cross_ambient_hysteresis_adjustment({
          multiplier = -1,
          divider = 1
        })
      })
    }
  }
})
