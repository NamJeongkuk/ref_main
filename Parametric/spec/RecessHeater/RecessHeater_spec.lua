local RecessHeater = require 'RecessHeater/RecessHeater'
local VariableSweatHeater = require 'VariableSweatHeater/VariableSweatHeater'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.utilities.TypedString

describe('RecessHeater', function()
  local recess_heater = RecessHeater(core_mock)
  local variable_sweat_heater = VariableSweatHeater(core_mock)

  local some_variable_sweat_heater = variable_sweat_heater({
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

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      variable_sweat_heater = some_variable_sweat_heater
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(recess_heater, generate_config())
  end)

  it('should assert if variable_sweat_heater is not of type string variable_sweat_heater', function()
    should_fail_with('variable_sweat_heater must be a typed string with type variable_sweat_heater, but is a number', function()
      recess_heater(generate_config({
        variable_sweat_heater = 2
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type recess_heater', function()
    local expected = remove_whitespace([[
      structure(
        structure(
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
      )
    ]])

    local actual = recess_heater({
      variable_sweat_heater = some_variable_sweat_heater
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('recess_heater'))
  end)

  it('should memoize', function()
    should_memoize_calls(recess_heater, generate_config())
  end)

end)
