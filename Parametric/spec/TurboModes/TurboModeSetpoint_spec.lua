local TurboModeSetpoint = require 'TurboModes/TurboModeSetpoint'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('TurboModeSetpoint', function()
  local turbo_mode_setpoint = TurboModeSetpoint(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      fresh_food_turbo_cool_setpoint = TypedString('fresh_food_turbo_cool_setpoint', 'fresh_food_turbo_cool_setpoint'),
      freezer_turbo_freeze_setpoint = TypedString('freezer_turbo_freeze_setpoint', 'freezer_turbo_freeze_setpoint')
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(turbo_mode_setpoint, generate_config())
  end)

  it('should constrain all arguments', function()
    should_fail_with('fresh_food_turbo_cool_setpoint must be a typed string with type fresh_food_turbo_cool_setpoint, but is a number', function()
      turbo_mode_setpoint(generate_config({
         fresh_food_turbo_cool_setpoint = -1
      }))
    end)

    should_fail_with('freezer_turbo_freeze_setpoint must be a typed string with type freezer_turbo_freeze_setpoint, but is a number', function()
      turbo_mode_setpoint(generate_config({
         freezer_turbo_freeze_setpoint = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type adjusted_setpoint', function()
    local expected = remove_whitespace([[
      structure(
        pointer(fresh_food_turbo_cool_setpoint),
        pointer(freezer_turbo_freeze_setpoint)
      )
    ]])

    local actual = turbo_mode_setpoint(generate_config())

    assert(actual.is_of_type('turbo_mode_setpoint'))
    assert.equals(expected, remove_whitespace(tostring(actual)))
  end)
end)
