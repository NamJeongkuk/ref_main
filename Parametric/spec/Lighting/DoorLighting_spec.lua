local DoorLighting = require 'Lighting/DoorLighting'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_require_args = require 'lua-common'.utilities.should_require_args
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local TypedString = require 'lua-common'.util.TypedString

describe('DoorLighting', function()
  local door_lighting = DoorLighting(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      normal_operation_ramping_pwm_duty_cycle_percentage_bundle = TypedString('ramping_pwm_duty_cycle_percentage_bundle', 'ramping_pwm_duty_cycle_percentage_bundle')    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(door_lighting, generate_config())
  end)

  it('should constrain all arguments', function()
    should_fail_with('normal_operation_ramping_pwm_duty_cycle_percentage_bundle must be a typed string with type ramping_pwm_duty_cycle_percentage_bundle, but is a number', function()
      door_lighting(generate_config({
        normal_operation_ramping_pwm_duty_cycle_percentage_bundle = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type ramping_pwm_duty_cycle_percentage_bundle', function()
    local expected = remove_whitespace([[
      structure(
        pointer(ramping_pwm_duty_cycle_percentage_bundle)
      )
    ]])

    local actual = door_lighting(generate_config())

    assert(actual.is_of_type('door_lighting'))
    assert.equals(expected, remove_whitespace(tostring(actual)))
  end)
end)
