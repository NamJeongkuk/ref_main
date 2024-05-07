local IceCabinetFanStartup = require 'IceCabinet/IceCabinetFanStartup'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local fan_speed_type = require 'Fan.FanSpeedType'

describe('IceCabinetFanStartup', function()
  local ice_cabinet_fan_startup = IceCabinetFanStartup(core_mock)
  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      startup_speed = 'medium_speed',
      startup_time_in_seconds = 3
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(ice_cabinet_fan_startup, generate_config())
  end)

  it('should require fan speed to be valid fan speed', function()
    should_fail_with("startup_speed='not a valid fan speed' must be in the set { 'off_speed', 'super_low_speed', 'low_speed', 'medium_speed', 'high_speed', 'super_high_speed' }", function()
      ice_cabinet_fan_startup(generate_config({
        startup_speed = 'not a valid fan speed'
      }))
    end)
  end)

  it('should assert if startup_time_in_seconds is not in range', function()
    should_fail_with('startup_time_in_seconds=-1 must be in [0, 255]', function()
      ice_cabinet_fan_startup(generate_config({
        startup_time_in_seconds = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type ice_cabinet_fan_startup', function()
    local expected = remove_whitespace([[
      structure(
        u16(]] .. fan_speed_type.medium_speed .. [[),
        u8(3)
      )
    ]])

    local actual = ice_cabinet_fan_startup({
      startup_speed = 'medium_speed',
      startup_time_in_seconds = 3
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('ice_cabinet_fan_startup'))
  end)

  it('should memoize', function()
    should_memoize_calls(ice_cabinet_fan_startup, generate_config())
  end)
end)
