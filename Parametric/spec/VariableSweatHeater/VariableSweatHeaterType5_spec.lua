local VariableSweatHeaterType5 = require 'VariableSweatHeater/VariableSweatHeaterType5'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('VariableSweatHeaterType5', function()
  local variable_sweat_heater = VariableSweatHeaterType5(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      temperature_coefficient = 2100,
      humidity_coefficient = 20,
      fresh_food_coefficient = 10,
      temperature_squared_coefficient = 12,
      humidity_squared_coefficient = 10,
      fresh_food_squared_coefficient = 11,
      temperature_humidity_coefficient = 20,
      temperature_fresh_food_coefficient = 22,
      humidity_fresh_food_coefficient = 10,
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

  it('should assert if humidity_coefficient is not in range', function()
    should_fail_with('humidity_coefficient=2147483648 must be in [-2147483648, 2147483647]', function()
      variable_sweat_heater(generate_config({
        humidity_coefficient = 2147483648
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

  it('should assert if humidity_squared_coefficient is not in range', function()
    should_fail_with('humidity_squared_coefficient=32768 must be in [-32768, 32767]', function()
      variable_sweat_heater(generate_config({
        humidity_squared_coefficient = 32768
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

  it('should assert if temperature_humidity_coefficient is not in range', function()
    should_fail_with('temperature_humidity_coefficient=32768 must be in [-32768, 32767]', function()
      variable_sweat_heater(generate_config({
        temperature_humidity_coefficient = 32768
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

  it('should assert if humidity_fresh_food_coefficient is not in range', function()
    should_fail_with('humidity_fresh_food_coefficient=32768 must be in [-32768, 32767]', function()
      variable_sweat_heater(generate_config({
        humidity_fresh_food_coefficient = 32768
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
        i32(20),
        i32(10),
        i16(12),
        i16(10),
        i16(11),
        i16(20),
        i16(22),
        i16(10),
        i32(-205000)
      )
    ]])

    local actual = variable_sweat_heater({
      temperature_coefficient = 2100,
      humidity_coefficient = 20,
      fresh_food_coefficient = 10,
      temperature_squared_coefficient = 12,
      humidity_squared_coefficient = 10,
      fresh_food_squared_coefficient = 11,
      temperature_humidity_coefficient = 20,
      temperature_fresh_food_coefficient = 22,
      humidity_fresh_food_coefficient = 10,
      intercept_coefficient = -205000
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('variable_sweat_heater_type_5'))
  end)

  it('should memoize', function()
    should_memoize_calls(variable_sweat_heater, generate_config())
  end)
end)