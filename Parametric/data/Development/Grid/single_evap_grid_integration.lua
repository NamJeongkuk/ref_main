return grid({
  grid_id = 'grid_single_evap',
  grid_deltas = import("../Grid/grid_deltas_integration.lua"),
  fresh_food_cross_ambient_hysteresis_coefficient_in_degfx1000_over_degf = 500,
  grid_invalid_freezer_thermistor_fallback_row = 3,
  grid_invalid_fresh_food_thermistor_fallback_column = 1,
  fresh_food_minimum_cross_ambient_adjusted_hysteresis_in_degfx100 = 200,
  grid_periodic_run_rate_in_msec = 1000
})
