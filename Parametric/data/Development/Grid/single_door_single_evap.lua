return grid({
  grid_id = 'grid_single_door_single_evap',
  grid_deltas = import("../Grid/grid_deltas.lua"),
  -- The fallback row and column are used to keep us within
  -- a specific row for all fresh food and column for all freezer
  grid_invalid_freezer_thermistor_fallback_row = 6,
  grid_invalid_fresh_food_thermistor_fallback_column = 1,
  fresh_food_minimum_cross_ambient_adjusted_hysteresis_in_degfx100 = 0,
  fresh_food_cross_ambient_hysteresis_coefficient_in_degfx1000_over_degf = 0,
  grid_periodic_run_rate_in_msec = 1000
})
