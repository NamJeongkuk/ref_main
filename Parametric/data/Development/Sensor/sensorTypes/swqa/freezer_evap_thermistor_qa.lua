return sensor_type({
  mapping_table = import("../../temperature_sensor_15k_resistor_qa.lua"),
  fallback_value_in_degfx100 = -4100,
  exponentially_moving_average_filter_alpha_numerator = 1,
  exponentially_moving_average_filter_alpha_denominator = 10,
  exponentially_moving_average_filter_window_size = 10,
  slew_rate_filter_enabled = false,
  slew_rate_filter_slew_rate_in_degfx100_per_second = 0,
  fallback_good_reading_counter_max = 100,
  fallback_bad_reading_counter_max = 100,
  enable_external_sensor_check = false,
  discoverable = false
})
