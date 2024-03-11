local FreshFoodNonHeatedCycleDefrost = require 'NonHeatedCycleDefrost/FreshFoodNonHeatedCycleDefrost'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString
local fan_speed_type = require 'Fan.FanSpeedType'

describe('ConvertibleCompartment', function()
  local fresh_food_non_heated_cycle_defrost = FreshFoodNonHeatedCycleDefrost(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      fan_speed_compressor_off_enable = true,
      selaed_system_valve_change_enable = false,
      default_time_valve_change_in_minutes = 10,
      sealed_system_valve_change_slope_in_minutesx10_per_degf = 20,
      max_time_valve_change_in_minutes = 30,
      min_time_valve_change_in_minutes = 1,
      fan_speed_compressor_off = 'low_speed',
      fan_speed_valve_change = 'low_speed'
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(fresh_food_non_heated_cycle_defrost, generate_config())
  end)

  it('should assert if fan_speed_compressor_off_enable is not a bool', function()
    should_fail_with('fan_speed_compressor_off_enable must be of type boolean but is of type string', function()
      fresh_food_non_heated_cycle_defrost(generate_config({
        fan_speed_compressor_off_enable = "📮 true 📮"
      }))
    end)
  end)

  it('should assert if selaed_system_valve_change_enable is not a bool', function()
    should_fail_with('selaed_system_valve_change_enable must be of type boolean but is of type string', function()
      fresh_food_non_heated_cycle_defrost(generate_config({
        selaed_system_valve_change_enable = "📮 false 📮"
      }))
    end)
  end)

  it('should assert if default_time_valve_change_in_minutes is not in range', function()
    should_fail_with('default_time_valve_change_in_minutes=1000000 must be in [0, 255]', function()
      fresh_food_non_heated_cycle_defrost(generate_config({
        default_time_valve_change_in_minutes = 1000000
      }))
    end)
  end)

  it('should assert if sealed_system_valve_change_slope_in_minutesx10_per_degf is not in range', function()
    should_fail_with('sealed_system_valve_change_slope_in_minutesx10_per_degf=1000000 must be in [0, 255]', function()
      fresh_food_non_heated_cycle_defrost(generate_config({
        sealed_system_valve_change_slope_in_minutesx10_per_degf = 1000000
      }))
    end)
  end)
  it('should assert if max_time_valve_change_in_minutes is not in range', function()
    should_fail_with('max_time_valve_change_in_minutes=1000000 must be in [0, 255]', function()
      fresh_food_non_heated_cycle_defrost(generate_config({
        max_time_valve_change_in_minutes = 1000000
      }))
    end)
  end)

  it('should assert if min_time_valve_change_in_minutes is not in range', function()
    should_fail_with('min_time_valve_change_in_minutes=1000000 must be in [0, 255]', function()
      fresh_food_non_heated_cycle_defrost(generate_config({
        min_time_valve_change_in_minutes = 1000000
      }))
    end)
  end)

  it('should require fan_speed_compressor_off to be valid fan speed', function()
    should_fail_with("fan_speed_compressor_off='not a valid fan speed' must be in the set { 'off_speed', 'super_low_speed', 'low_speed', 'medium_speed', 'high_speed', 'super_high_speed' }", function()
      fresh_food_non_heated_cycle_defrost(generate_config({
        fan_speed_compressor_off = 'not a valid fan speed'
      }))
    end)
  end)

  it('should require fan_speed_valve_change to be valid fan speed', function()
    should_fail_with("fan_speed_valve_change='not a valid fan speed' must be in the set { 'off_speed', 'super_low_speed', 'low_speed', 'medium_speed', 'high_speed', 'super_high_speed' }", function()
      fresh_food_non_heated_cycle_defrost(generate_config({
        fan_speed_valve_change = 'not a valid fan speed'
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type convertible_compartment', function()
    local expected = remove_whitespace([[
      structure(
        bool(true),
        bool(false),
        u8(10),
        u8(20),
        u8(30),
        u8(1),
        u8(]] .. fan_speed_type.low_speed .. [[),
        u8(]] .. fan_speed_type.low_speed .. [[)
      )
    ]])

    local actual = fresh_food_non_heated_cycle_defrost(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('fresh_food_non_heated_cycle_defrost'))
  end)

  it('should memoize', function()
    should_memoize_calls(fresh_food_non_heated_cycle_defrost, generate_config())
  end)
end)
