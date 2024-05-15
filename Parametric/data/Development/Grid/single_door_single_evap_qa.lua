return grid({
  grid_id = grid_id_type.grid_single_door_single_evap,
  -- The fallback row and column are used to keep us within
  -- a specific row for all fresh food and column for all freezer
  grid_invalid_freezer_thermistor_fallback_row = 6,
  grid_invalid_fresh_food_thermistor_fallback_column = 1,
  grid_periodic_run_rate_in_msec = 1000
})
