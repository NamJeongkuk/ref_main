local UserSetpointData = require 'Setpoints/UserSetpointData'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('userSetpointData', function()
  local userSetpointData = UserSetpointData(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      fresh_food_user_setpoint = TypedString('user_setpoint', 'fresh_food_user_setpoint'),
      freezer_user_setpoint = TypedString('user_setpoint', 'freezer_user_setpoint')
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(userSetpointData, generate_config())
  end)

  it('should constrain all arguments', function()
    should_fail_with('fresh_food_user_setpoint must be a typed string with type user_setpoint, but is a number', function()
      userSetpointData(generate_config({
        fresh_food_user_setpoint = -1
      }))
    end)

    should_fail_with('freezer_user_setpoint must be a typed string with type user_setpoint, but is a number', function()
      userSetpointData(generate_config({
        freezer_user_setpoint = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type user_setpoint_data', function()
    local expected = remove_whitespace([[
      structure(
        pointer(fresh_food_user_setpoint),
        pointer(freezer_user_setpoint)
      )
    ]])

    local actual = userSetpointData(generate_config())

    assert(actual.is_of_type('user_setpoint_data'))
    assert.equals(expected, remove_whitespace(tostring(actual)))
  end)
end)
