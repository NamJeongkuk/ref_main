local HumiditySensorType = require 'Sensor.HumiditySensorType'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('humidity_sensor_type', function()
  local humidity_sensor_type = HumiditySensorType(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      mapping_table = {
        data = {
          { raw = 4550, mapped = 79770 },
          { raw = 4600, mapped = 81274 },
          { raw = 4650, mapped = 82796 },
          { raw = 4700, mapped = 84339 },
          { raw = 4750, mapped = 85901 },
          { raw = 4800, mapped = 87484 },
        },
        invalid_value = 3000000
      },
      fallback_value_in_percent_humidityx100 = 7500,
      exponentially_moving_average_filter_alpha_numerator = 1,
      exponentially_moving_average_filter_alpha_denominator = 10, -- alpha = alphaNum / alphaDenom
      exponentially_moving_average_filter_window_size = 10,
      slew_rate_filter_enabled = true,
      slew_rate_filter_slew_rate_in_humidityx100_per_second = 10,
      fallback_good_reading_counter_max = 100,
      fallback_bad_reading_counter_max = 100,
      enable_external_sensor_check = true
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(humidity_sensor_type, generate_config())
  end)

  it('should fail with incorrect arguments', function()
    should_fail_with('missing argument mapping_table.invalid_value', function()
      humidity_sensor_type(generate_config({
        mapping_table = {
          data = {}
        }
      }))
    end)

    should_fail_with('missing argument mapping_table.data', function()
      humidity_sensor_type(generate_config({
        mapping_table = {
          invalid_value = 0
        }
      }))
    end)

    should_fail_with('mapping_table.data[1].raw=-1 must be in [0, 65535]', function()
      local config = generate_config({
        mapping_table = {
          data = { { raw = -1, mapped = 3994 } },
          invalid_value = 3000000
        }
      })
      humidity_sensor_type(config)
    end)

    should_fail_with('mapping_table.data[1].mapped=-1000000000000 must be in [0, 4294967295]', function()
      local config = generate_config({
        mapping_table = {
          data = { { raw = 4992, mapped = -1000000000000 } },
          invalid_value = 3000000
        }
      })
      humidity_sensor_type(config)
    end)

    should_fail_with('mapping_table.invalid_value=30000000 must be equal to 3000000', function()
      local config = generate_config({
        mapping_table = {
          data = { { raw = 4992, mapped = 3994 } },
          invalid_value = 30000000
        }
      })
      humidity_sensor_type(config)
    end)

    should_fail_with('fallback_value_in_percent_humidityx100=-410000 must be in [0, 65535]', function()
      local config = generate_config({
        mapping_table = {
          data = { { raw = 4992, mapped = 3994 } },
          invalid_value = 3000000
        },
        fallback_value_in_percent_humidityx100 = -410000
      })
      humidity_sensor_type(config)
    end)

    should_fail_with('exponentially_moving_average_filter_alpha_numerator=-1 must be in [0, 255]', function()
      local config = generate_config({
        mapping_table = {
          data = { { raw = 4992, mapped = 3994 } },
          invalid_value = 3000000
        },
        fallback_value_in_percent_humidityx100 = 7500,
        exponentially_moving_average_filter_alpha_numerator = -1
      })
      humidity_sensor_type(config)
    end)

    should_fail_with('exponentially_moving_average_filter_alpha_denominator=-1 must be in [0, 255]', function()
      local config = generate_config({
        mapping_table = {
          data = { { raw = 4992, mapped = 3994 } },
          invalid_value = 3000000
        },
        fallback_value_in_percent_humidityx100 = 7500,
        exponentially_moving_average_filter_alpha_numerator = 1,
        exponentially_moving_average_filter_alpha_denominator = -1
      })
      humidity_sensor_type(config)
    end)

    should_fail_with('exponentially_moving_average_filter_window_size=-1 must be in [0, 255]', function()
      local config = generate_config({
        mapping_table = {
          data = { { raw = 4992, mapped = 3994 } },
          invalid_value = 3000000
        },
        fallback_value_in_percent_humidityx100 = 7500,
        exponentially_moving_average_filter_alpha_numerator = 1,
        exponentially_moving_average_filter_alpha_denominator = 10,
        exponentially_moving_average_filter_window_size = -1
      })
      humidity_sensor_type(config)
    end)

    should_fail_with('slew_rate_filter_slew_rate_in_humidityx100_per_second=-100000 must be in [0, 65535]', function()
      local config = generate_config({
        mapping_table = {
          data = { { raw = 4992, mapped = 3994 } },
          invalid_value = 3000000
        },
        fallback_value_in_percent_humidityx100 = 7500,
        exponentially_moving_average_filter_alpha_numerator = 1,
        exponentially_moving_average_filter_alpha_denominator = 10,
        exponentially_moving_average_filter_window_size = 10,
        slew_rate_filter_slew_rate_in_humidityx100_per_second = -100000
      })
      humidity_sensor_type(config)
    end)

    should_fail_with('fallback_good_reading_counter_max=-1 must be in [0, 65535]', function()
      local config = generate_config({
        mapping_table = {
          data = { { raw = 4992, mapped = 3994 } },
          invalid_value = 3000000
        },
        fallback_value_in_percent_humidityx100 = 7500,
        exponentially_moving_average_filter_alpha_numerator = 1,
        exponentially_moving_average_filter_alpha_denominator = 10,
        exponentially_moving_average_filter_window_size = 10,
        slew_rate_filter_enabled = true,
        slew_rate_filter_slew_rate_in_humidityx100_per_second = 10,
        fallback_good_reading_counter_max = -1
      })
      humidity_sensor_type(config)
    end)

    should_fail_with('fallback_bad_reading_counter_max=-1 must be in [0, 65535]', function()
      local config = generate_config({
        mapping_table = {
          data = { { raw = 4992, mapped = 3994 } },
          invalid_value = 3000000
        },
        fallback_value_in_percent_humidityx100 = 7500,
        exponentially_moving_average_filter_alpha_numerator = 1,
        exponentially_moving_average_filter_alpha_denominator = 10,
        exponentially_moving_average_filter_window_size = 10,
        slew_rate_filter_enabled = true,
        slew_rate_filter_slew_rate_in_humidityx100_per_second = 10,
        fallback_good_reading_counter_max = 100,
        fallback_bad_reading_counter_max = -1
      })
      humidity_sensor_type(config)
    end)
  end)

  it('should generate a typed string with the correct data and type humidity_sensor_type', function()
    local expected = remove_whitespace([[
      structure(
        pointer(
          structure(
            pointer(
              structure(
                structure(i32(0),i32(3000000)),
                structure(i32(4549),i32(3000000)),
                structure(i32(4550),i32(79770)),
                structure(i32(4600),i32(81274)),
                structure(i32(4650),i32(82796)),
                structure(i32(4700),i32(84339)),
                structure(i32(4750),i32(85901)),
                structure(i32(4800),i32(87484)),
                structure(i32(4801),i32(3000000)),
                structure(i32(65535),i32(3000000))
              )
            ),
            size(10)
          )
        ),
        u16(7500),
        u8(1),
        u8(10),
        u8(10),
        structure(
          bool(true),
          u16(10)
        ),
        u16(100),
        u16(100),
        bool(true))
    ]])

    local actual = humidity_sensor_type(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('humidity_sensor_type'))
  end)

  it('should memoize', function()
    should_memoize_calls(humidity_sensor_type, generate_config())
  end)

end)
