local RfidFilterUpdateRate = require 'FilterData/RfidFilterUpdateRate'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('RfidFilterUpdateRate', function()
  local rfid_filter_update_rate = RfidFilterUpdateRate(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      door_open_filter_read_frequency_in_seconds = 30,
      door_just_closed_filter_read_frequency_in_seconds = 60,
      door_closed_filter_update_time_in_minutes = 30,
      door_closed_rfid_error_detected_read_frequency_in_seconds = 10,
      door_just_closed_filter_read_timeout_in_seconds = 120,
      rfid_filter_write_timeout_in_minutes = 2
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(rfid_filter_update_rate, generate_config())
  end)

  it('should assert if door_open_filter_read_frequency_in_seconds is not in range', function()
    should_fail_with('door_open_filter_read_frequency_in_seconds=-1 must be in [0, 255]', function()
      rfid_filter_update_rate(generate_config({
        door_open_filter_read_frequency_in_seconds = -1
      }))
    end)
  end)

  it('should assert if door_just_closed_filter_read_frequency_in_seconds is not in range', function()
    should_fail_with('door_just_closed_filter_read_frequency_in_seconds=-1 must be in [0, 255]', function()
      rfid_filter_update_rate(generate_config({
        door_just_closed_filter_read_frequency_in_seconds = -1
      }))
    end)
  end)

  it('should assert if door_closed_filter_update_time_in_minutes is not in range', function()
    should_fail_with('door_closed_filter_update_time_in_minutes=-1 must be in [0, 255]', function()
      rfid_filter_update_rate(generate_config({
        door_closed_filter_update_time_in_minutes = -1
      }))
    end)
  end)

  it('should assert if door_closed_rfid_error_detected_read_frequency_in_seconds is not in range', function()
    should_fail_with('door_closed_rfid_error_detected_read_frequency_in_seconds=-1 must be in [0, 255]', function()
      rfid_filter_update_rate(generate_config({
        door_closed_rfid_error_detected_read_frequency_in_seconds = -1
      }))
    end)
  end)

  it('should assert if door_just_closed_filter_read_timeout_in_seconds is not in range', function()
    should_fail_with('door_just_closed_filter_read_timeout_in_seconds=-1 must be in [0, 255]', function()
      rfid_filter_update_rate(generate_config({
        door_just_closed_filter_read_timeout_in_seconds = -1
      }))
    end)
  end)

  it('should assert if rfid_filter_write_timeout_in_minutes is not in range', function()
    should_fail_with('rfid_filter_write_timeout_in_minutes=-1 must be in [0, 255]', function()
      rfid_filter_update_rate(generate_config({
        rfid_filter_write_timeout_in_minutes = -1
      }))
    end)
  end)

  it('should require door_just_closed_filter_read_frequency_in_seconds to be less than door_just_closed_filter_read_timeout_in_seconds', function()
    should_fail_with('door_just_closed_filter_read_frequency_in_seconds=15 must be < door_just_closed_filter_read_timeout_in_seconds=10', function()
      rfid_filter_update_rate(generate_config({
        door_just_closed_filter_read_timeout_in_seconds = 10,
        door_just_closed_filter_read_frequency_in_seconds = 15
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for rfid filter read write rate', function()
    local expected = remove_whitespace([[
        structure(
          u8(10),
          u8(11),
          u8(12),
          u8(13),
          u8(14),
          u8(15)
        )
      ]])

    local actual = rfid_filter_update_rate({
      door_open_filter_read_frequency_in_seconds = 10,
      door_just_closed_filter_read_frequency_in_seconds = 11,
      door_closed_filter_update_time_in_minutes = 12,
      door_closed_rfid_error_detected_read_frequency_in_seconds = 13,
      door_just_closed_filter_read_timeout_in_seconds = 14,
      rfid_filter_write_timeout_in_minutes = 15
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('rfid_filter_update_rate'))
  end)

  it('should memoize', function()
    should_memoize_calls(rfid_filter_update_rate, generate_config())
  end)
end)
