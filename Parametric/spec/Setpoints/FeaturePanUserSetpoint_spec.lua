local FeaturePanUserSetpoint = require 'Setpoints/FeaturePanUserSetpoint'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString
local feature_pan_mode_label = require 'FeaturePan/FeaturePanModeLabel'

describe('FeaturePanUserSetpoint', function()
  local feature_pan_user_setpoint = FeaturePanUserSetpoint(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      max_temperature_setpoint_in_degfx100 = 5,
      min_temperature_setpoint_in_degfx100 = -6,
      default_temperature_setpoint_in_degfx100 = 0,
      mode_label = feature_pan_mode_label.refrigerator
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(feature_pan_user_setpoint, generate_config())
  end)

  it('should assert if max_temperature_setpoint_in_degfx100 is not in range', function()
    should_fail_with('max_temperature_setpoint_in_degfx100=32768 must be in [-32768, 32767]', function()
      feature_pan_user_setpoint(generate_config({
        max_temperature_setpoint_in_degfx100 = 32768
      }))
    end)
  end)

  it('should assert if min_temperature_setpoint_in_degfx100 is not in range', function()
    should_fail_with('min_temperature_setpoint_in_degfx100=-32769 must be in [-32768, 32767]', function()
      feature_pan_user_setpoint(generate_config({
        min_temperature_setpoint_in_degfx100 = -32769
      }))
    end)
  end)

  it('should assert if default_temperature_setpoint_in_degfx100 is not in range', function()
    should_fail_with('default_temperature_setpoint_in_degfx100=32767 must be in [-6, 5]', function()
      feature_pan_user_setpoint(generate_config({
        default_temperature_setpoint_in_degfx100 = 32767
      }))
    end)

    should_fail_with('default_temperature_setpoint_in_degfx100=-32768 must be in [-6, 5]', function()
      feature_pan_user_setpoint(generate_config({
        default_temperature_setpoint_in_degfx100 = -32768
      }))
    end)
  end)

  it('should assert if mode_label is not a feature_pan_mode_label', function()
    should_fail_with('mode_label=not_a_type is not in the feature_pan_mode_label enumeration, allowable values: beverages, chiller, drinks, freezer, ice_cream, meat, produce, refrigerator, snacks, soft_freeze, wine', function()
      feature_pan_user_setpoint(generate_config({
        mode_label = 'not_a_type'
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for feature_pan_user_setpoint', function()
    local expected = remove_whitespace([[
      structure(
        i16(-6),
        i16(5),
        i16(0),
        u8(1),
        u8(0)
      )
    ]])

    local actual = feature_pan_user_setpoint(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('feature_pan_user_setpoint'))
  end)

  it('should memoize', function()
    should_memoize_calls(feature_pan_user_setpoint, generate_config())
  end)
end)
