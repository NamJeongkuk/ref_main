local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

local function error_if_first_argument_is_gtoet_second(value1, value2, stringVal1, stringVal2)
  if value1>=value2 then
    error(stringVal1 .. value1 .. ' must be < ' .. stringVal2 .. value2)
  end
end

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'rfid_filter_update_rate' },
      structure(
        u8(config.door_open_filter_read_frequency_in_seconds),
        u8(config.door_just_closed_filter_read_frequency_in_seconds),
        u8(config.door_closed_filter_update_time_in_minutes),
        u8(config.door_closed_rfid_error_detected_read_frequency_in_seconds),
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
        door_closed_rfid_error_detected_read_frequency_in_seconds = { constraint.u8 },
        door_just_closed_filter_read_timeout_in_seconds = { constraint.u8 },
        rfid_filter_write_timeout_in_minutes = { constraint.u8 }
      }
    )

    error_if_first_argument_is_gtoet_second(
      config.door_just_closed_filter_read_frequency_in_seconds,
      config.door_just_closed_filter_read_timeout_in_seconds,
      'door_just_closed_filter_read_frequency_in_seconds=',
      'door_just_closed_filter_read_timeout_in_seconds=')

    return generate(config)
  end
end
