return humidity_sensor_type({
  mapping_table = import("../../humidity_sensor_dummy.lua"),
  fallback_value_in_percent_humidityx100 = 5000,
  exponentially_moving_average_filter_alpha_numerator = 1,
  exponentially_moving_average_filter_alpha_denominator = 10,
  exponentially_moving_average_filter_window_size = 10,
  slew_rate_filter_enabled = true,
  slew_rate_filter_slew_rate_in_humidityx100_per_second = 10,
  fallback_good_reading_counter_max = 10,
  fallback_bad_reading_counter_max = 11,
  enable_external_sensor_check = true,
  discoverable = false
})
