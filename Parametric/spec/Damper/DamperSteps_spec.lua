local DamperSteps = require 'Damper/DamperSteps'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('DamperSteps', function()
  local damper_steps = DamperSteps(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      steps_to_open = 650,
      steps_to_close = 700
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(damper_steps, generate_config())
  end)

  it('should assert if steps_to_open is not in range', function()
    should_fail_with('steps_to_open=65536 must be in [0, 65535]', function()
      damper_steps(generate_config({
        steps_to_open = 65536
      }))
    end)
  end)

  it('should assert if steps_to_close is not in range', function()
    should_fail_with('steps_to_close=65536 must be in [0, 65535]', function()
      damper_steps(generate_config({
        steps_to_close = 65536
      }))
    end)
  end)


  it('should generate a typed string with the correct data and type for damper steps', function()
    local expected = remove_whitespace([[
      structure(
        u16(650),
        u16(700)
      )
    ]])

    local actual = damper_steps({
      steps_to_open = 650,
      steps_to_close = 700,
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('damper_steps'))
  end)

  it('should memoize', function()
    should_memoize_calls(damper_steps, generate_config())
  end)
end)
