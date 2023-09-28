local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'rfid_filter_update_rate' },
      structure(
        u8(config.door_open_filter_read_frequency_in_seconds),
        u8(config.door_just_closed_filter_read_frequency_in_seconds),
        u8(config.door_closed_filter_update_time_in_minutes),
        u8(config.door_closed_rfid_error_detected_update_frequency_in_seconds),
        u8(config.door_just_closed_filter_read_timeout_in_seconds),
        u8(config.rfid_filter_write_timeout_in_minutes)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        door_open_filter_read_frequency_in_seconds = { constraint.u8 },
        door_just_closed_filter_read_frequency_in_seconds = { constraint.u8 },
        door_closed_filter_update_time_in_minutes = { constraint.u8 },
        door_closed_rfid_error_detected_update_frequency_in_seconds = { constraint.u8 },
        door_just_closed_filter_read_timeout_in_seconds = { constraint.u8 },
        rfid_filter_write_timeout_in_minutes = { constraint.u8 },
      }
    )
    return generate(config)
  end
end
