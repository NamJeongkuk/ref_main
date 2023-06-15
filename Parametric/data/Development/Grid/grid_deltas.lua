local grid_line_temps = import("../Grid/grid_line_temperatures.lua")

return grid_deltas({
  deltas = {
    fresh_food_grid_deltas = {
      fresh_food_no_freeze_limit_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.fresh_food_grid_temperatures_in_degfx100.fresh_food_no_freeze_limit_in_degfx100,
        correction = 'offset'
      }),
      fresh_food_low_hyst_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.fresh_food_grid_temperatures_in_degfx100.fresh_food_low_hyst_in_degfx100,
        correction = 'adjusted_setpoint'
      }),
      fresh_food_set_point_delta_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.fresh_food_grid_temperatures_in_degfx100.fresh_food_set_point_delta_in_degfx100,
        correction = 'shift'
      }),
      fresh_food_high_hyst_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.fresh_food_grid_temperatures_in_degfx100.fresh_food_high_hyst_in_degfx100,
        correction = 'raw_setpoint'
      }),
      fresh_food_extra_high_hyst_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.fresh_food_grid_temperatures_in_degfx100.fresh_food_extra_high_hyst_in_degfx100,
        correction = 'adjusted_setpoint'
      }),
      fresh_food_super_high_hyst_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.fresh_food_grid_temperatures_in_degfx100.fresh_food_super_high_hyst_in_degfx100,
        correction = 'adjusted_setpoint'
      })
    },
    freezer_grid_deltas = {
      freezer_low_hyst_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.freezer_grid_temperatures_in_degfx100.freezer_low_hyst_in_degfx100,
        correction = 'adjusted_setpoint'
      }),
      freezer_setpoint_delta_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.freezer_grid_temperatures_in_degfx100.freezer_setpoint_delta_in_degfx100,
        correction = 'offset'
      }),
      freezer_high_hyst_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.freezer_grid_temperatures_in_degfx100.freezer_high_hyst_in_degfx100,
        correction = 'raw_setpoint'
      }),
      freezer_extra_high_hyst_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.freezer_grid_temperatures_in_degfx100.freezer_extra_high_hyst_in_degfx100,
        correction = 'shift'
      }),
      freezer_super_high_hyst_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.freezer_grid_temperatures_in_degfx100.freezer_super_high_hyst_in_degfx100,
        correction = 'adjusted_setpoint'
      }),
      freezer_extreme_high_hyst_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.freezer_grid_temperatures_in_degfx100.freezer_extreme_high_hyst_in_degfx100,
        correction = 'offset'
      })
    }
  }
})
