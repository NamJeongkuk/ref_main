local ConvertibleDamperStepData = require 'Damper/ConvertibleDamperStepData'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString
local DamperSteps = require 'Damper/DamperSteps'

describe('ConvertibleDamperStepData', function()
  local convertible_damper_step_data = ConvertibleDamperStepData(core_mock)
  local damper_steps = DamperSteps(core_mock)

  local damper_steps_as_fresh_food = damper_steps({
    steps_to_open = 650,
    steps_to_close = 700
  })

  local damper_steps_as_freezer = damper_steps({
    steps_to_open = 651,
    steps_to_close = 701
  })

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      damper_steps_as_fresh_food = damper_steps_as_fresh_food,
      damper_steps_as_freezer = damper_steps_as_freezer
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(convertible_damper_step_data, generate_config())
  end)

  it('should assert if fields are not damper steps', function()
    should_fail_with('damper_steps_as_fresh_food must be a typed string with type damper_steps, has type(s) blah', function()
      convertible_damper_step_data(generate_config({
        damper_steps_as_fresh_food = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('damper_steps_as_freezer must be a typed string with type damper_steps, has type(s) blah', function()
      convertible_damper_step_data(generate_config({
        damper_steps_as_freezer = TypedString('blah', 'some_blah')
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for damper steps', function()
    local expected = remove_whitespace([[
      structure(
        structure(
          u16(650),
          u16(700)
        ),
        structure(
          u16(651),
          u16(701)
        )
      )
    ]])

    local actual = convertible_damper_step_data({
      damper_steps_as_fresh_food = damper_steps_as_fresh_food,
      damper_steps_as_freezer = damper_steps_as_freezer
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('convertible_damper_step_data'))
  end)

  it('should memoize', function()
    should_memoize_calls(convertible_damper_step_data, generate_config())
  end)
end)
