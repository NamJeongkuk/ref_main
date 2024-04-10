local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local FeaturePan = require 'FeaturePan/FeaturePan'

describe('FeaturePan', function()
  local feature_pan = FeaturePan(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      feature_pan_warmup_slope_lower_limit_in_degfx100_per_minute = 100,
      feature_pan_warmup_slope_upper_limit_in_degfx100_per_minute = 200,
      feature_pan_heater_high_ambient_temperature_limit = 8000,
      fresh_food_fan_deli_cooling_speed = 'low_speed'
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(feature_pan, generate_config())
  end)

  it('should assert if feature_pan_warmup_slope_lower_limit_in_degfx100_per_minute is not in range', function()
    should_fail_with('feature_pan_warmup_slope_lower_limit_in_degfx100_per_minute=32768 must be in [-32768, 32767]', function()
      feature_pan(generate_config({
        feature_pan_warmup_slope_lower_limit_in_degfx100_per_minute = 32768
      }))
    end)
  end)

  it('should assert if feature_pan_warmup_slope_upper_limit_in_degfx100_per_minute is not in range', function()
    should_fail_with('feature_pan_warmup_slope_upper_limit_in_degfx100_per_minute=32768 must be in [-32768, 32767]', function()
      feature_pan(generate_config({
        feature_pan_warmup_slope_upper_limit_in_degfx100_per_minute = 32768
      }))
    end)
  end)

  it('should assert if feature_pan_heater_high_ambient_temperature_limit is not in range', function()
    should_fail_with('feature_pan_heater_high_ambient_temperature_limit=32768 must be in [-32768, 32767]', function()
      feature_pan(generate_config({
        feature_pan_heater_high_ambient_temperature_limit = 32768
      }))
    end)
  end)

  it('should require fresh_food_fan_deli_cooling_speed to be valid fan speed', function()
    should_fail_with("fresh_food_fan_deli_cooling_speed='maximum_overdrive' must be in the set { 'off_speed', 'super_low_speed', 'low_speed', 'medium_speed', 'high_speed', 'super_high_speed' }", function()
      feature_pan(generate_config({
        fresh_food_fan_deli_cooling_speed = 'maximum_overdrive'
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for single damper', function()
    local expected = remove_whitespace([[
      structure(
        i16(100),
        i16(200),
        i16(8000),
        u16(2)
      )
    ]])

    local actual = feature_pan({
      feature_pan_warmup_slope_lower_limit_in_degfx100_per_minute = 100,
      feature_pan_warmup_slope_upper_limit_in_degfx100_per_minute = 200,
      feature_pan_heater_high_ambient_temperature_limit = 8000,
      fresh_food_fan_deli_cooling_speed = 'low_speed'
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('feature_pan'))
  end)

  it('should memoize', function()
    should_memoize_calls(feature_pan, generate_config())
  end)
end)
