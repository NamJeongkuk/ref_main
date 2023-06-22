local VariableSweatHeaterType1 = require 'VariableSweatHeater/VariableSweatHeaterType1'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('VariableSweatHeaterType1', function()
  local variable_sweat_heater = VariableSweatHeaterType1(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      temperature_coefficient = 2100,
      temperature_squared_coefficient = 12,
      intercept_coefficient = -205000
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(variable_sweat_heater, generate_config())
  end)

  it('should assert if temperature_coefficient is not in range', function()
    should_fail_with('temperature_coefficient=2147483648 must be in [-2147483648, 2147483647]', function()
      variable_sweat_heater(generate_config({
        temperature_coefficient = 2147483648
      }))
    end)
  end)

  it('should assert if temperature_squared_coefficient is not in range', function()
    should_fail_with('temperature_squared_coefficient=32768 must be in [-32768, 32767]', function()
      variable_sweat_heater(generate_config({
        temperature_squared_coefficient = 32768
      }))
    end)
  end)

  it('should assert if intercept_coefficient is not in range', function()
    should_fail_with('intercept_coefficient=2147483648 must be in [-2147483648, 2147483647]', function()
      variable_sweat_heater(generate_config({
        intercept_coefficient = 2147483648
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for variable_sweat_heater', function()
    local expected = remove_whitespace([[
      structure(
        i32(2100),
        i16(12),
        i32(-205000)
      )
    ]])

    local actual = variable_sweat_heater({
      temperature_coefficient = 2100,
      temperature_squared_coefficient = 12,
      intercept_coefficient = -205000
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('variable_sweat_heater_type_1'))
  end)

  it('should memoize', function()
    should_memoize_calls(variable_sweat_heater, generate_config())
  end)
end)
