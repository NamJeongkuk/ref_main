local VariableSweatHeater = require 'VariableSweatHeater/VariableSweatHeater'
local VariableSweatHeaterType1 = require 'VariableSweatHeater/VariableSweatHeaterType1'
local VariableSweatHeaterType2 = require 'VariableSweatHeater/VariableSweatHeaterType2'
local VariableSweatHeaterType3 = require 'VariableSweatHeater/VariableSweatHeaterType3'
local VariableSweatHeaterType4 = require 'VariableSweatHeater/VariableSweatHeaterType4'
local VariableSweatHeaterType5 = require 'VariableSweatHeater/VariableSweatHeaterType5'
local VariableSweatHeaterType6 = require 'VariableSweatHeater/VariableSweatHeaterType6'
local VariableSweatHeaterType7 = require 'VariableSweatHeater/VariableSweatHeaterType7'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('VariableSweatHeater', function()
  local variable_sweat_heater = VariableSweatHeater(core_mock)
  local variable_sweat_heater_type_1 = VariableSweatHeaterType1(core_mock)
  local variable_sweat_heater_type_2 = VariableSweatHeaterType2(core_mock)
  local variable_sweat_heater_type_3 = VariableSweatHeaterType3(core_mock)
  local variable_sweat_heater_type_4 = VariableSweatHeaterType4(core_mock)
  local variable_sweat_heater_type_5 = VariableSweatHeaterType5(core_mock)
  local variable_sweat_heater_type_6 = VariableSweatHeaterType6(core_mock)
  local variable_sweat_heater_type_7 = VariableSweatHeaterType7(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      fallback_duty_cycle_in_percent = 100,
      variable_sweat_heater = TypedString('variable_sweat_heater_type_1', 'variable_sweat_heater_type_1'),
    },overrides or {})
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

  it('should assert if variable_sweat_heater is not of type string variable_sweat_heater_type_1', function()
    local error = 'variable_sweat_heater must be a typed string with type variable_sweat_heater_type_1, but is a number ' ..
      'OR variable_sweat_heater must be a typed string with type variable_sweat_heater_type_2, but is a number ' ..
      'OR variable_sweat_heater must be a typed string with type variable_sweat_heater_type_3, but is a number ' ..
      'OR variable_sweat_heater must be a typed string with type variable_sweat_heater_type_4, but is a number ' ..
      'OR variable_sweat_heater must be a typed string with type variable_sweat_heater_type_5, but is a number ' ..
      'OR variable_sweat_heater must be a typed string with type variable_sweat_heater_type_6, but is a number ' ..
      'OR variable_sweat_heater must be a typed string with type variable_sweat_heater_type_7, but is a number'

    should_fail_with(error, function()
      variable_sweat_heater(generate_config({
        variable_sweat_heater = 2
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for variable_sweat_heater', function()
    local expected = remove_whitespace([[
      structure(
        u8(1),
        u8(100),
        pointer(variable_sweat_heater_type_1)
      )
      ]])

    local actual = variable_sweat_heater({
      fallback_duty_cycle_in_percent = 100,
      variable_sweat_heater = TypedString('variable_sweat_heater_type_1', 'variable_sweat_heater_type_1')
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('variable_sweat_heater'))
  end)

  it('should memoize', function()
    should_memoize_calls(variable_sweat_heater, generate_config())
  end)
end)
