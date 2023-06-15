local VariableSweatHeater = require 'VariableSweatHeater/VariableSweatHeater'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('VariableSweatHeater', function()
  local variable_sweat_heater = VariableSweatHeater(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      fallback_duty_cycle_in_percent = 100,
      heater_control_type = 7,
      temperature_coefficient = 2100,
      humidity_coefficient = 0,
      fresh_food_coefficient = 0,
      freezer_coefficient = 0,
      temperature_squared_coefficient = 12,
      humidity_squared_coefficient = 0,
      fresh_food_squared_coefficient = 0,
      freezer_squared_coefficient = 0,
      temperature_humidity_coefficient = 0,
      temperature_fresh_food_coefficient = 0,
      temperature_freezer_coefficient = 0,
      humidity_fresh_food_coefficient = 0,
      humidity_freezer_coefficient = 0,
      fresh_food_freezer_coefficient = 0,
      intercept_coefficient = -205000
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(variable_sweat_heater, generate_config())
  end)

  it('should assert if fallback_duty_cycle_in_percent is not in range', function()
    should_fail_with('fallback_duty_cycle_in_percent=101 must be in [0, 100]', function()
      variable_sweat_heater(generate_config({
        fallback_duty_cycle_in_percent = 101
      }))
    end)
  end)

  it('should assert if heater_control_type is not in range', function()
    should_fail_with('heater_control_type=8 must be in [1, 7]', function()
      variable_sweat_heater(generate_config({
        heater_control_type = 8
      }))
    end)
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

  it('should assert if freezer_coefficient is not in range', function()
    should_fail_with('freezer_coefficient=2147483648 must be in [-2147483648, 2147483647]', function()
      variable_sweat_heater(generate_config({
        freezer_coefficient = 2147483648
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

  it('should assert if freezer_squared_coefficient is not in range', function()
    should_fail_with('freezer_squared_coefficient=32768 must be in [-32768, 32767]', function()
      variable_sweat_heater(generate_config({
        freezer_squared_coefficient = 32768
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

  it('should assert if temperature_freezer_coefficient is not in range', function()
    should_fail_with('temperature_freezer_coefficient=32768 must be in [-32768, 32767]', function()
      variable_sweat_heater(generate_config({
        temperature_freezer_coefficient = 32768
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

  it('should assert if humidity_freezer_coefficient is not in range', function()
    should_fail_with('humidity_freezer_coefficient=32768 must be in [-32768, 32767]', function()
      variable_sweat_heater(generate_config({
        humidity_freezer_coefficient = 32768
      }))
    end)
  end)

  it('should assert if fresh_food_freezer_coefficient is not in range', function()
    should_fail_with('fresh_food_freezer_coefficient=32768 must be in [-32768, 32767]', function()
      variable_sweat_heater(generate_config({
        fresh_food_freezer_coefficient = 32768
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
        u8(100),
        u8(7),
        i32(2100),
        i32(0),
        i32(0),
        i32(0),
        i16(12),
        i16(0),
        i16(0),
        i16(0),
        i16(0),
        i16(0),
        i16(0),
        i16(0),
        i16(0),
        i16(0),
        i32(-205000)
      )
    ]])

    local actual = variable_sweat_heater({
      fallback_duty_cycle_in_percent = 100,
      heater_control_type = 7,
      temperature_coefficient = 2100,
      humidity_coefficient = 0,
      fresh_food_coefficient = 0,
      freezer_coefficient = 0,
      temperature_squared_coefficient = 12,
      humidity_squared_coefficient = 0,
      fresh_food_squared_coefficient = 0,
      freezer_squared_coefficient = 0,
      temperature_humidity_coefficient = 0,
      temperature_fresh_food_coefficient = 0,
      temperature_freezer_coefficient = 0,
      humidity_fresh_food_coefficient = 0,
      humidity_freezer_coefficient = 0,
      fresh_food_freezer_coefficient = 0,
      intercept_coefficient = -205000
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('variable_sweat_heater'))
  end)

  it('should memoize', function()
    should_memoize_calls(variable_sweat_heater, generate_config())
  end)
end)
