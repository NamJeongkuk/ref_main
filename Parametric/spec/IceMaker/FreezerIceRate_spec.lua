local FreezerIceRate = require 'IceMaker/FreezerIceRate'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local fan_speed_type = require 'Fan.FanSpeedType'

describe('FreezerIceRate', function()
  local freezer_ice_rate = FreezerIceRate(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      time_in_minutes = 2,
      freezer_setpoint_in_degfx100 = 250,
      freezer_evaporator_fan_speed = 'medium_speed'
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(freezer_ice_rate, generate_config())
  end)

  it('should assert if time_in_minutes is not in range', function()
    should_fail_with('time_in_minutes=-1 must be in [0, 255]', function()
      freezer_ice_rate(generate_config({
        time_in_minutes = -1
      }))
    end)
  end)

  it('should assert if freezer_setpoint_in_degfx100 is not in range', function()
    should_fail_with('freezer_setpoint_in_degfx100=32768 must be in [-32768, 32767]', function()
      freezer_ice_rate(generate_config({
        freezer_setpoint_in_degfx100 = 32768
      }))
    end)
  end)

  it('should assert if freezer_evaporator_fan_speed is not in range', function()
    should_fail_with("freezer_evaporator_fan_speed=-1 must be in the set { 'off_speed', 'super_low_speed', 'low_speed', 'medium_speed', 'high_speed', 'super_high_speed' }", function()
      freezer_ice_rate(generate_config({
        freezer_evaporator_fan_speed = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for freezer ice rate', function()
    local expected = remove_whitespace([[
        structure(
          u8(2),
          i16(250),
          u8(]] .. fan_speed_type.medium_speed ..[[)
        )
      ]])

    local actual = freezer_ice_rate({
      time_in_minutes = 2,
      freezer_setpoint_in_degfx100 = 250,
      freezer_evaporator_fan_speed = 'medium_speed'
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('freezer_ice_rate'))
  end)

  it('should memoize', function()
    should_memoize_calls(freezer_ice_rate, generate_config())
  end)
end)
