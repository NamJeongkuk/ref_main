local UserSetpoint = require 'Setpoints/UserSetpoint'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('UserSetpoint', function()
  local user_setpoint = UserSetpoint(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      min_temperature_setpoint_in_degf = -6,
      max_temperature_setpoint_in_degf = 5,
      default_temperature_setpoint_in_degf = 0
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(user_setpoint, generate_config())
  end)

  it('should assert if min_temperature_setpoint_in_degf is not in range', function()
    should_fail_with('min_temperature_setpoint_in_degf=-129 must be in [-128, 127]', function()
      user_setpoint(generate_config({
        min_temperature_setpoint_in_degf = -129
      }))
    end)
  end)

  it('should assert if max_temperature_setpoint_in_degf is not in range', function()
    should_fail_with('max_temperature_setpoint_in_degf=128 must be in [-128, 127]', function()
      user_setpoint(generate_config({
        max_temperature_setpoint_in_degf = 128
      }))
    end)
  end)

  it('should assert if default_temperature_setpoint_in_degf is not in range', function()
    should_fail_with('default_temperature_setpoint_in_degf=6 must be in [-6, 5]', function()
      user_setpoint(generate_config({
        min_temperature_setpoint_in_degf = -6,
        max_temperature_setpoint_in_degf = 5,
        default_temperature_setpoint_in_degf = 6
      }))
    end)

    should_fail_with('default_temperature_setpoint_in_degf=-7 must be in [-6, 5]', function()
      user_setpoint(generate_config({
        min_temperature_setpoint_in_degf = -6,
        max_temperature_setpoint_in_degf = 5,
        default_temperature_setpoint_in_degf = -7
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for user_setpoint', function()
    local expected = remove_whitespace([[
      structure(
        i8(-6),
        i8(5),
        i8(0)
      )
    ]])

    local actual = user_setpoint(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('user_setpoint'))
  end)

  it('should memoize', function()
    should_memoize_calls(user_setpoint, generate_config())
  end)
end)
