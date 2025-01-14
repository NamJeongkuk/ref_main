local grid_line_temps = import("../../Grid/fresh_food_and_freezer_grid_line_temperatures.lua")

return sensor_type({
  mapping_table = import("../temperature_sensor_15k_resistor.lua"),
  fallback_value_in_degfx100 = grid_line_temps.freezer_grid_temperatures_in_degfx100.freezer_high_hyst_in_degfx100 + 50,
  exponentially_moving_average_filter_alpha_numerator = 1,
  exponentially_moving_average_filter_alpha_denominator = 10,
  exponentially_moving_average_filter_window_size = 10,
  slew_rate_filter_enabled = true,
  slew_rate_filter_slew_rate_in_degfx100_per_second = 10,
  fallback_good_reading_counter_max = 100,
  fallback_bad_reading_counter_max = 100,
  enable_external_sensor_check = false,
  discoverable = false
})
