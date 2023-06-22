local VariableSweatHeaterType3 = require 'VariableSweatHeater/VariableSweatHeaterType3'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('VariableSweatHeaterType3', function()
  local variable_sweat_heater = VariableSweatHeaterType3(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      temperature_coefficient = 2100,
      fresh_food_coefficient = 10,
      temperature_squared_coefficient = 12,
      fresh_food_squared_coefficient = 11,
      temperature_fresh_food_coefficient = 22,
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

  it('should assert if fresh_food_coefficient is not in range', function()
    should_fail_with('fresh_food_coefficient=2147483648 must be in [-2147483648, 2147483647]', function()
      variable_sweat_heater(generate_config({
        fresh_food_coefficient = 2147483648
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

  it('should assert if fresh_food_squared_coefficient is not in range', function()
    should_fail_with('fresh_food_squared_coefficient=32768 must be in [-32768, 32767]', function()
      variable_sweat_heater(generate_config({
        fresh_food_squared_coefficient = 32768
      }))
    end)
  end)

  it('should assert if temperature_fresh_food_coefficient is not in range', function()
    should_fail_with('temperature_fresh_food_coefficient=32768 must be in [-32768, 32767]', function()
      variable_sweat_heater(generate_config({
        temperature_fresh_food_coefficient = 32768
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
        i32(10),
        i16(12),
        i16(11),
        i16(22),
        i32(-205000)
      )
    ]])

    local actual = variable_sweat_heater({
      temperature_coefficient = 2100,
      fresh_food_coefficient = 10,
      temperature_squared_coefficient = 12,
      fresh_food_squared_coefficient = 11,
      temperature_fresh_food_coefficient = 22,
      intercept_coefficient = -205000
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('variable_sweat_heater_type_3'))
  end)

  it('should memoize', function()
    should_memoize_calls(variable_sweat_heater, generate_config())
  end)
end)
