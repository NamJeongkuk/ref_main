return grid({
  grid_id = grid_id_type.grid_ice_cabinet,
  grid_deltas = import("../Grid/grid_deltas_ice_cabinet.lua"),
  grid_invalid_thermistor_fallback_block = 4,
  minimum_cross_ambient_adjusted_hysteresis_in_degfx100 = 0,
  cross_ambient_hysteresis_coefficient_in_degfx1000_over_degf = 0,
  grid_periodic_run_rate_in_msec = 1000
})
