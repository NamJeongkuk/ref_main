local FreshFoodThermalOffset = require 'Setpoints/FreshFoodThermalOffset'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('FreshFoodThermalOffset', function()
  local fresh_food_thermal_offset = FreshFoodThermalOffset(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      cabinet_offset_in_degfx100 = 100,
      fresh_food_pulldown_offset_in_degfx100 = 30,
      cross_ambient_offset = TypedString('cross_ambient_offset', 'cross_ambient_offset'),
      high_ambient_offset = TypedString('high_ambient_offset', 'high_ambient_offset'),
      setpoint_offset = TypedString('setpoint_offset', 'setpoint_offset'),
      shift_offset = TypedString('shift_offset', 'shift_offset')
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(fresh_food_thermal_offset, generate_config())
  end)

  it('should assert if cabinet_offset_in_degfx100 is not in range', function()
    should_fail_with('cabinet_offset_in_degfx100=32768 must be in [-32768, 32767]', function()
      fresh_food_thermal_offset(generate_config({
        cabinet_offset_in_degfx100 = 32768
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('cross_ambient_offset must be a typed string with type cross_ambient_offset, but is a number', function()
      fresh_food_thermal_offset(generate_config({
        cross_ambient_offset = -1
      }))
    end)

    should_fail_with('high_ambient_offset must be a typed string with type high_ambient_offset, but is a number', function()
        fresh_food_thermal_offset(generate_config({
          high_ambient_offset = -1
        }))
    end)

    should_fail_with('setpoint_offset must be a typed string with type setpoint_offset, but is a number', function()
      fresh_food_thermal_offset(generate_config({
        setpoint_offset = -1
      }))
    end)

    should_fail_with('shift_offset must be a typed string with type shift_offset, but is a number', function()
      fresh_food_thermal_offset(generate_config({
        shift_offset = -1
      }))
    end)
  end)

  it('should assert if fresh_food_pulldown_offset_in_degfx100 is not in range', function()
    should_fail_with('fresh_food_pulldown_offset_in_degfx100=32768 must be in [-32768, 32767]', function()
      fresh_food_thermal_offset(generate_config({
        fresh_food_pulldown_offset_in_degfx100 = 32768
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for fresh food adjusted setpoint', function()
    local expected = remove_whitespace([[
      structure(
        i16(100),
        i16(30),
        pointer(cross_ambient_offset),
        pointer(high_ambient_offset),
        pointer(setpoint_offset),
        pointer(shift_offset)
    )
    ]])

    local actual = fresh_food_thermal_offset(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('fresh_food_thermal_offset'))
  end)

  it('should memoize', function()
    should_memoize_calls(fresh_food_thermal_offset, generate_config())
  end)
end)
