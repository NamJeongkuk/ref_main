local grid_line_temps = import("../Grid/fresh_food_and_freezer_grid_line_temperatures_qa.lua")

return grid_deltas({
  deltas = {
    fresh_food_grid_deltas = {
      fresh_food_no_freeze_limit_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.fresh_food_grid_temperatures_in_degfx100.fresh_food_no_freeze_limit_in_degfx100,
        correction = grid_line_correction.offset,
        cross_ambient_hysteresis_adjustment = cross_ambient_hysteresis_adjustment({
          multiplier = 1,
          divider = 1
        })
      }),
      fresh_food_low_hyst_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.fresh_food_grid_temperatures_in_degfx100.fresh_food_low_hyst_in_degfx100,
        correction = grid_line_correction.adjusted_setpoint,
        cross_ambient_hysteresis_adjustment = cross_ambient_hysteresis_adjustment({
          multiplier = 1,
          divider = 1
        })
      }),
      fresh_food_set_point_delta_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.fresh_food_grid_temperatures_in_degfx100.fresh_food_set_point_delta_in_degfx100,
        correction = grid_line_correction.adjusted_setpoint,
        cross_ambient_hysteresis_adjustment = cross_ambient_hysteresis_adjustment({
          multiplier = grid_line_temps.fresh_food_grid_temperatures_in_degfx100.fresh_food_set_point_delta_in_degfx100,
          divider = grid_line_temps.fresh_food_grid_temperatures_in_degfx100.fresh_food_low_hyst_in_degfx100
        })
      }),
      fresh_food_high_hyst_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.fresh_food_grid_temperatures_in_degfx100.fresh_food_high_hyst_in_degfx100,
        correction = grid_line_correction.adjusted_setpoint,
        cross_ambient_hysteresis_adjustment = cross_ambient_hysteresis_adjustment({
          multiplier = -1,
          divider = 1
        })
      }),
      fresh_food_extra_high_hyst_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.fresh_food_grid_temperatures_in_degfx100.fresh_food_extra_high_hyst_in_degfx100,
        correction = grid_line_correction.adjusted_setpoint,
        cross_ambient_hysteresis_adjustment = cross_ambient_hysteresis_adjustment({
          multiplier = -1,
          divider = 1
        })
      }),
      fresh_food_super_high_hyst_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.fresh_food_grid_temperatures_in_degfx100.fresh_food_super_high_hyst_in_degfx100,
        correction = grid_line_correction.adjusted_setpoint,
        cross_ambient_hysteresis_adjustment = cross_ambient_hysteresis_adjustment({
          multiplier = -1,
          divider = 1
        })
      })
    },
    freezer_grid_deltas = {
      freezer_low_hyst_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.freezer_grid_temperatures_in_degfx100.freezer_low_hyst_in_degfx100,
        correction = grid_line_correction.adjusted_setpoint,
        cross_ambient_hysteresis_adjustment = import("../Grid/cross_ambient_hysteresis_adjustment_disabled.lua")
      }),
      freezer_setpoint_delta_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.freezer_grid_temperatures_in_degfx100.freezer_setpoint_delta_in_degfx100,
        correction = grid_line_correction.adjusted_setpoint,
        cross_ambient_hysteresis_adjustment = import("../Grid/cross_ambient_hysteresis_adjustment_disabled.lua")
      }),
      freezer_high_hyst_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.freezer_grid_temperatures_in_degfx100.freezer_high_hyst_in_degfx100,
        correction = grid_line_correction.adjusted_setpoint,
        cross_ambient_hysteresis_adjustment = import("../Grid/cross_ambient_hysteresis_adjustment_disabled.lua")
      }),
      freezer_extra_high_hyst_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.freezer_grid_temperatures_in_degfx100.freezer_extra_high_hyst_in_degfx100,
        correction = grid_line_correction.adjusted_setpoint,
        cross_ambient_hysteresis_adjustment = import("../Grid/cross_ambient_hysteresis_adjustment_disabled.lua")
      }),
      freezer_super_high_hyst_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.freezer_grid_temperatures_in_degfx100.freezer_super_high_hyst_in_degfx100,
        correction = grid_line_correction.adjusted_setpoint,
        cross_ambient_hysteresis_adjustment = import("../Grid/cross_ambient_hysteresis_adjustment_disabled.lua")
      }),
      freezer_extreme_high_hyst_in_degfx100 = gridline({
        delta_in_degfx100 = grid_line_temps.freezer_grid_temperatures_in_degfx100.freezer_extreme_high_hyst_in_degfx100,
        correction = grid_line_correction.offset,
        cross_ambient_hysteresis_adjustment = import("../Grid/cross_ambient_hysteresis_adjustment_disabled.lua")
      })
    }
  }
})
