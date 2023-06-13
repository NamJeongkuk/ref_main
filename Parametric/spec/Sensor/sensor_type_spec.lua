local SensorType = require 'Sensor.SensorType'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('sensor_type', function()
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
                structure(i32(0),i32(30000)),
                structure(i32(4991),i32(30000)),
                structure(i32(4992),i32(-3994)),
                structure(i32(6976),i32(-3147)),
                structure(i32(9344),i32(-2245)),
                structure(i32(12096),i32(-1343)),
                structure(i32(15168),i32(-475)),
                structure(i32(18688),i32(405)),
                structure(i32(22272),i32(1231)),
                structure(i32(26048),i32(2056)),
                structure(i32(29952),i32(2893)),
                structure(i32(33792),i32(3723)),
                structure(i32(37632),i32(4585)),
                structure(i32(41280),i32(5458)),
                structure(i32(44864),i32(6399)),
                structure(i32(48128),i32(7366)),
                structure(i32(51136),i32(8400)),
                structure(i32(53888),i32(9532)),
                structure(i32(56192),i32(10697)),
                structure(i32(58048),i32(11867)),
                structure(i32(59584),i32(13090)),
                structure(i32(60544),i32(14011)),
                structure(i32(60545),i32(30000)),
                structure(i32(65535),i32(30000))
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
