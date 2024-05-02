local Setpoint = require 'Setpoints/Setpoint'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('Setpoint', function()
  local setpoint = Setpoint(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      setpoint_zone = TypedString('setpoint_zone', 'setpoint_zone'),
      user_setpoint_data = TypedString('user_setpoint_data', 'user_setpoint_data')
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(setpoint, generate_config())
  end)

  it('should constrain all arguments', function()
    should_fail_with('setpoint_zone must be a typed string with type setpoint_zone, but is a number', function()
      setpoint(generate_config({
        setpoint_zone = -1
      }))
    end)

    should_fail_with('user_setpoint_data must be a typed string with type user_setpoint_data, but is a number', function()
      setpoint(generate_config({
        user_setpoint_data = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type setpoint', function()
    local expected = remove_whitespace([[
      structure(
        pointer(setpoint_zone),
        pointer(user_setpoint_data)
      )
    ]])

    local actual = setpoint(generate_config())

    assert(actual.is_of_type('setpoint'))
    assert.equals(expected, remove_whitespace(tostring(actual)))
  end)
end)
