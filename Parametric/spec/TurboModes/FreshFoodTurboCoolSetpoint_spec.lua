local FreshFoodTurboCoolSetpoint = require 'TurboModes/FreshFoodTurboCoolSetpoint'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('FreshFoodTurboCoolSetpoint', function()
  local fresh_food_turbo_cool_setpoint = FreshFoodTurboCoolSetpoint(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      turbo_cool_setpoint_in_degfx100 = 3500,
      turbo_cool_on_time_in_minutes = 360
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(fresh_food_turbo_cool_setpoint, generate_config())
  end)

  it('should assert if turbo_cool_setpoint_in_degfx100 is not in range', function()
    should_fail_with('turbo_cool_setpoint_in_degfx100=32768 must be in [-32768, 32767]', function()
      fresh_food_turbo_cool_setpoint(generate_config({
        turbo_cool_setpoint_in_degfx100 = 32768
      }))
    end)
  end)

  it('should assert if turbo_cool_on_time_in_minutes is not in range', function()
    should_fail_with('turbo_cool_on_time_in_minutes=65536 must be in [0, 65535]', function()
      fresh_food_turbo_cool_setpoint(generate_config({
        turbo_cool_on_time_in_minutes = 65536
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for fresh food turbo cool setpoint', function()
    local expected = remove_whitespace([[
      structure(
        i16(3500),
        u16(360)
      )
    ]])

    local actual = fresh_food_turbo_cool_setpoint(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('fresh_food_turbo_cool_setpoint'))
  end)

  it('should memoize', function()
    should_memoize_calls(fresh_food_turbo_cool_setpoint, generate_config())
  end)
end)
