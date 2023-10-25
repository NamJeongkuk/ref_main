local SensorType = require 'Sensor.SensorType'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('SensorType', function()
  local sensor_type = SensorType(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      mapping_table = {
        data = {
          { raw = 4992, mapped = -3994 },
          { raw = 6976, mapped = -3147 },
          { raw = 9344, mapped = -2245 },
          { raw = 12096, mapped = -1343 },
          { raw = 15168, mapped = -475 },
          { raw = 18688, mapped = 405 },
          { raw = 22272, mapped = 1231 },
          { raw = 26048, mapped = 2056 },
          { raw = 29952, mapped = 2893 },
          { raw = 33792, mapped = 3723 },
          { raw = 37632, mapped = 4585 },
          { raw = 41280, mapped = 5458 },
          { raw = 44864, mapped = 6399 },
          { raw = 48128, mapped = 7366 },
          { raw = 51136, mapped = 8400 },
          { raw = 53888, mapped = 9532 },
          { raw = 56192, mapped = 10697 },
          { raw = 58048, mapped = 11867 },
          { raw = 59584, mapped = 13090 },
          { raw = 60544, mapped = 14011 }
        },
        invalid_value = 30000
      },
      fallback_value_in_degfx100 = -4100,
      exponentially_moving_average_filter_alpha_numerator = 1,
      exponentially_moving_average_filter_alpha_denominator = 10, -- alpha = alphaNum / alphaDenom
      exponentially_moving_average_filter_window_size = 10,
      slew_rate_filter_enabled = true,
      slew_rate_filter_slew_rate_in_degfx100_per_second = 10,
      fallback_good_reading_counter_max = 100,
      fallback_bad_reading_counter_max = 100,
      enable_external_sensor_check = true
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(sensor_type, generate_config())
  end)

  it('should fail with incorrect arguments', function()
    should_fail_with('missing argument mapping_table.invalid_value', function()
      sensor_type(generate_config({
        mapping_table = {
          data = {}
        }
      }))
    end)

    should_fail_with('missing argument mapping_table.data', function()
      sensor_type(generate_config({
        mapping_table = {
          invalid_value = 0
        }
      }))
    end)

    should_fail_with('mapping_table.data[1].raw=-1 must be in [0, 65535]', function()
      local config = generate_config({
        mapping_table = {
          data = { { raw = -1, mapped = -3994 } },
          invalid_value = 30000
        }
      })
      sensor_type(config)
    end)

    should_fail_with('mapping_table.invalid_value=30000000 must be equal to 30000', function()
      local config = generate_config({
        mapping_table = {
          data = { { raw = 4992, mapped = 3994 } },
          invalid_value = 30000000
        }
      })
      sensor_type(config)
    end)

    should_fail_with('fallback_value_in_degfx100=-410000 must be in [-32768, 32767]', function()
      local config = generate_config({
        mapping_table = {
          data = { { raw = 4992, mapped = 3994 } },
          invalid_value = 30000
        },
        fallback_value_in_degfx100 = -410000
      })
      sensor_type(config)
    end)

    should_fail_with('exponentially_moving_average_filter_alpha_numerator=-1 must be in [0, 255]', function()
      local config = generate_config({
        mapping_table = {
          data = { { raw = 4992, mapped = 3994 } },
          invalid_value = 30000
        },
        fallback_value_in_degfx100 = -4100,
        exponentially_moving_average_filter_alpha_numerator = -1
      })
      sensor_type(config)
    end)

    should_fail_with('exponentially_moving_average_filter_alpha_denominator=-1 must be in [0, 255]', function()
      local config = generate_config({
        mapping_table = {
          data = { { raw = 4992, mapped = 3994 } },
          invalid_value = 30000
        },
        fallback_value_in_degfx100 = -4100,
        exponentially_moving_average_filter_alpha_numerator = 1,
        exponentially_moving_average_filter_alpha_denominator = -1
      })
      sensor_type(config)
    end)

    should_fail_with('exponentially_moving_average_filter_window_size=-1 must be in [0, 255]', function()
      local config = generate_config({
        mapping_table = {
          data = { { raw = 4992, mapped = 3994 } },
          invalid_value = 30000
        },
        fallback_value_in_degfx100 = -4100,
        exponentially_moving_average_filter_alpha_numerator = 1,
        exponentially_moving_average_filter_alpha_denominator = 10,
        exponentially_moving_average_filter_window_size = -1
      })
      sensor_type(config)
    end)

    should_fail_with('slew_rate_filter_slew_rate_in_degfx100_per_second=-100000 must be in [0, 65535]', function()
      local config = generate_config({
        mapping_table = {
          data = { { raw = 4992, mapped = 3994 } },
          invalid_value = 30000
        },
        fallback_value_in_degfx100 = -4100,
        exponentially_moving_average_filter_alpha_numerator = 1,
        exponentially_moving_average_filter_alpha_denominator = 10,
        exponentially_moving_average_filter_window_size = 10,
        slew_rate_filter_slew_rate_in_degfx100_per_second = -100000
      })
      sensor_type(config)
    end)

    should_fail_with('fallback_good_reading_counter_max=-1 must be in [0, 65535]', function()
      local config = generate_config({
        mapping_table = {
          data = { { raw = 4992, mapped = 3994 } },
          invalid_value = 30000
        },
        fallback_value_in_degfx100 = -4100,
        exponentially_moving_average_filter_alpha_numerator = 1,
        exponentially_moving_average_filter_alpha_denominator = 10,
        exponentially_moving_average_filter_window_size = 10,
        slew_rate_filter_slew_rate_in_degfx100_per_second = 10,
        slew_rate_filter_enabled = true,
        fallback_good_reading_counter_max = -1
      })
      sensor_type(config)
    end)

    should_fail_with('fallback_bad_reading_counter_max=-1 must be in [0, 65535]', function()
      local config = generate_config({
        mapping_table = {
          data = { { raw = 4992, mapped = 3994 } },
          invalid_value = 30000
        },
        fallback_value_in_degfx100 = -4100,
        exponentially_moving_average_filter_alpha_numerator = 1,
        exponentially_moving_average_filter_alpha_denominator = 10,
        exponentially_moving_average_filter_window_size = 10,
        slew_rate_filter_slew_rate_in_degfx100_per_second = 10,
        slew_rate_filter_enabled = true,
        fallback_good_reading_counter_max = 100,
        fallback_bad_reading_counter_max = -1
      })
      sensor_type(config)
    end)
  end)

  it('should generate a typed string with the correct data and type sensor_type', function()
    local expected = remove_whitespace([[
      structure(
        pointer(
          structure(
            pointer(
              structure(
                structure(u16(0),i16(30000)),
                structure(u16(4991),i16(30000)),
                structure(u16(4992),i16(-3994)),
                structure(u16(6976),i16(-3147)),
                structure(u16(9344),i16(-2245)),
                structure(u16(12096),i16(-1343)),
                structure(u16(15168),i16(-475)),
                structure(u16(18688),i16(405)),
                structure(u16(22272),i16(1231)),
                structure(u16(26048),i16(2056)),
                structure(u16(29952),i16(2893)),
                structure(u16(33792),i16(3723)),
                structure(u16(37632),i16(4585)),
                structure(u16(41280),i16(5458)),
                structure(u16(44864),i16(6399)),
                structure(u16(48128),i16(7366)),
                structure(u16(51136),i16(8400)),
                structure(u16(53888),i16(9532)),
                structure(u16(56192),i16(10697)),
                structure(u16(58048),i16(11867)),
                structure(u16(59584),i16(13090)),
                structure(u16(60544),i16(14011)),
                structure(u16(60545),i16(30000)),
                structure(u16(65535),i16(30000))
              )
            ),
            size(24)
          )
        ),
        i16(-4100),
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

    local actual = sensor_type(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('sensor_type'))
  end)

  it('should memoize', function()
    should_memoize_calls(sensor_type, generate_config())
  end)

end)
