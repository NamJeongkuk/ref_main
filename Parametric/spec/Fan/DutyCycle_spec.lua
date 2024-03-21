local DutyCycle = require 'Fan/DutyCycle'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('DutyCycle', function()
  local duty_cycle = DutyCycle(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      duty_cycle = 100,
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(duty_cycle, generate_config())
  end)

  it('should assert if duty_cycle is not in range', function()
    should_fail_with('duty_cycle=102 must be in [0, 100]', function()
      duty_cycle(generate_config({
        duty_cycle = 102
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type duty_cycle', function()
    local expected = remove_whitespace([[
      structure(
        u8(0),
        u16(77)
      )
      ]])

    local actual = duty_cycle({
      duty_cycle = 77,
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('speed'))
  end)

  it('should memoize', function()
    should_memoize_calls(duty_cycle, generate_config())
  end)

end)
