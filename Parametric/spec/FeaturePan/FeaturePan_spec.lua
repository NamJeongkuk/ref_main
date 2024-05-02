local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local FeaturePan = require 'FeaturePan/FeaturePan'
local feature_pan_mode = require 'FeaturePan/FeaturePanMode'

describe('FeaturePan', function()
  local feature_pan = FeaturePan(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      feature_pan_warmup_slope_lower_limit_in_degfx100_per_minute = 100,
      feature_pan_warmup_slope_upper_limit_in_degfx100_per_minute = 200,
      feature_pan_heater_high_ambient_temperature_limit_degFx100 = 8000,
      feature_pan_pulldown_temperature_limit_in_degfx100 = 5000,
      fresh_food_fan_deli_cooling_speed = 'low_speed',
      feature_pan_default_mode = feature_pan_mode.mode1,
      feature_pan_min_allowed_mode = feature_pan_mode.mode1,
      feature_pan_max_allowed_mode = feature_pan_mode.mode5,
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

  it('should assert if feature_pan_heater_high_ambient_temperature_limit_degFx100 is not in range', function()
    should_fail_with('feature_pan_heater_high_ambient_temperature_limit_degFx100=32768 must be in [-32768, 32767]', function()
      feature_pan(generate_config({
        feature_pan_heater_high_ambient_temperature_limit_degFx100 = 32768
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

  it('should assert if feature_pan_pulldown_temperature_limit_in_degfx100 is not in range', function()
    should_fail_with('feature_pan_pulldown_temperature_limit_in_degfx100=32768 must be in [-32768, 32767]', function()
      feature_pan(generate_config({
        feature_pan_pulldown_temperature_limit_in_degfx100 = 32768
      }))
    end)
  end)

  it('should require feature_pan_default_mode to be valid feature_pan_mode', function()
    should_fail_with("feature_pan_default_mode=bruh moment is not in the feature_pan_mode enumeration, allowable values: mode1, mode2, mode3, mode4, mode5, mode6, mode7, off", function()
      feature_pan(generate_config({
        feature_pan_default_mode = 'bruh moment'
      }))
    end)
  end)

  it('should assert if feature_pan_default_mode is not in range', function()
    should_fail_with('feature_pan_default_mode=6 must be in [1, 5]', function()
      feature_pan(generate_config({
        feature_pan_default_mode = feature_pan_mode.mode6,
        feature_pan_min_allowed_mode = feature_pan_mode.mode1,
        feature_pan_max_allowed_mode = feature_pan_mode.mode5,
      }))
    end)
  end)

  it('should require feature_pan_max_allowed_mode to be greater than feature_pan_min_allowed_mode', function()
    should_fail_with("feature_pan_default_mode=2 must be in [5, 1]", function()
      feature_pan(generate_config({
        feature_pan_default_mode = feature_pan_mode.mode2,
        feature_pan_min_allowed_mode = feature_pan_mode.mode5,
        feature_pan_max_allowed_mode = feature_pan_mode.mode1,
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for single damper', function()
    local expected = remove_whitespace([[
      structure(
        i16(100),
        i16(200),
        i16(8000),
        i16(5000),
        u16(2),
        u8(1),
        u8(1),
        u8(5)
      )
    ]])

    local actual = feature_pan({
      feature_pan_warmup_slope_lower_limit_in_degfx100_per_minute = 100,
      feature_pan_warmup_slope_upper_limit_in_degfx100_per_minute = 200,
      feature_pan_heater_high_ambient_temperature_limit_degFx100 = 8000,
      feature_pan_pulldown_temperature_limit_in_degfx100 = 5000,
      fresh_food_fan_deli_cooling_speed = 'low_speed',
      feature_pan_default_mode = feature_pan_mode.mode1,
      feature_pan_min_allowed_mode = feature_pan_mode.mode1,
      feature_pan_max_allowed_mode = feature_pan_mode.mode5
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('feature_pan'))
  end)

  it('should memoize', function()
    should_memoize_calls(feature_pan, generate_config())
  end)
end)
