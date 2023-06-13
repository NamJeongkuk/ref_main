local grid_line_temps = import("../Grid/grid_line_temperatures.lua")

return grid_deltas({
  deltas = {
    fresh_food_grid_deltas = {
      fresh_food_no_freeze_limit = gridline({
        delta_in_degfx100 = grid_line_temps.fresh_food_grid_temperatures_in_degfx100.fresh_food_no_freeze_limit,
        correction = 'offset'
      }),
      fresh_food_low_hyst = gridline({
        delta_in_degfx100 = grid_line_temps.fresh_food_grid_temperatures_in_degfx100.fresh_food_low_hyst,
        correction = 'adjusted_setpoint'
      }),
      fresh_food_low_hyst_plus_delta = gridline({
        delta_in_degfx100 = grid_line_temps.fresh_food_grid_temperatures_in_degfx100.fresh_food_low_hyst_plus_delta,
        correction = 'shift'
      }),
      fresh_food_high_hyst = gridline({
        delta_in_degfx100 = grid_line_temps.fresh_food_grid_temperatures_in_degfx100.fresh_food_high_hyst,
        correction = 'raw_setpoint'
      }),
      fresh_food_extra_high_hyst = gridline({
        delta_in_degfx100 = grid_line_temps.fresh_food_grid_temperatures_in_degfx100.fresh_food_extra_high_hyst,
        correction = 'adjusted_setpoint'
      }),
      fresh_food_super_high_hyst = gridline({
        delta_in_degfx100 = grid_line_temps.fresh_food_grid_temperatures_in_degfx100.fresh_food_super_high_hyst,
        correction = 'adjusted_setpoint'
      })
    },
    freezer_grid_deltas = {
      freezer_low_hyst = gridline({
        delta_in_degfx100 = grid_line_temps.freezer_grid_temperatures_in_degfx100.freezer_low_hyst,
        correction = 'adjusted_setpoint'
      }),
      freezer_delta = gridline({
        delta_in_degfx100 = grid_line_temps.freezer_grid_temperatures_in_degfx100.freezer_delta,
        correction = 'offset'
      }),
      freezer_high_hyst = gridline({
        delta_in_degfx100 = grid_line_temps.freezer_grid_temperatures_in_degfx100.freezer_high_hyst,
        correction = 'raw_setpoint'
      }),
      freezer_extra_high_hyst = gridline({
        delta_in_degfx100 = grid_line_temps.freezer_grid_temperatures_in_degfx100.freezer_extra_high_hyst,
        correction = 'shift'
      }),
      freezer_super_high_hyst = gridline({
        delta_in_degfx100 = grid_line_temps.freezer_grid_temperatures_in_degfx100.freezer_super_high_hyst,
        correction = 'adjusted_setpoint'
      }),
      freezer_extreme_high_hyst = gridline({
        delta_in_degfx100 = grid_line_temps.freezer_grid_temperatures_in_degfx100.freezer_extreme_high_hyst,
        correction = 'offset'
      })
    }
  }
})
