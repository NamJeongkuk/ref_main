return rfid_filter_update_rate({
  door_open_filter_read_frequency_in_seconds = 30,
  door_just_closed_filter_read_frequency_in_seconds = 60,
  door_closed_filter_update_time_in_minutes = 30,
  door_closed_rfid_error_detected_update_frequency_in_seconds = 10,
  door_just_closed_filter_read_timeout_in_seconds = 120,
  rfid_filter_write_timeout_in_minutes = 2
})
