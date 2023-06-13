local AugerMotor = require 'IceMaker/AugerMotor'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('auger_motor', function()
  local auger_motor = AugerMotor(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      auger_motor_reverse_direction_delay_msec = 2,
      auger_motor_dispense_complete_msec = 5,
      auger_motor_controlled_by_mainboard = true
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(auger_motor, generate_config())
  end)

  it('should assert if auger_motor_reverse_direction_delay_msec is not in range', function()
    should_fail_with('auger_motor_reverse_direction_delay_msec=-1 must be in [0, 65535]', function()
      auger_motor(generate_config({
        auger_motor_reverse_direction_delay_msec = -1
      }))
    end)
  end)

  it('should assert if auger_motor_dispense_complete_msec is not in range', function()
    should_fail_with('auger_motor_dispense_complete_msec=-1 must be in [0, 65535]', function()
      auger_motor(generate_config({
        auger_motor_dispense_complete_msec = -1
      }))
    end)
  end)

  it('should assert if auger_motor_controlled_by_mainboard is not a bool', function()
    should_fail_with('auger_motor_controlled_by_mainboard must be of type boolean but is of type number', function()
      auger_motor(generate_config({
        auger_motor_controlled_by_mainboard = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for freezer ice rate', function()
    local expected = remove_whitespace([[
        structure(
          u16(2),
          u16(5),
          bool(true)
        )
      ]])

    local actual = auger_motor({
      auger_motor_reverse_direction_delay_msec = 2,
      auger_motor_dispense_complete_msec = 5,
      auger_motor_controlled_by_mainboard = true
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('auger_motor'))
  end)

  it('should memoize', function()
    should_memoize_calls(auger_motor, generate_config())
  end)
end)
