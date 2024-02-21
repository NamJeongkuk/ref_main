local AdjustedSetpoint = require 'Setpoints/AdjustedSetpoint'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('AdjustedSetpoint', function()
  local adjusted_setpoint = AdjustedSetpoint(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      fresh_food_adjusted_setpoint = TypedString('fresh_food_adjusted_setpoint', 'fresh_food_adjusted_setpoint'),
      freezer_adjusted_setpoint = TypedString('freezer_adjusted_setpoint', 'freezer_adjusted_setpoint'),
      convertible_compartment_adjusted_setpoint = TypedString('convertible_compartment_adjusted_setpoint', 'convertible_compartment_adjusted_setpoint'),
      deli_pan_adjusted_setpoint = TypedString('deli_pan_adjusted_setpoint', 'deli_pan_adjusted_setpoint'),
      ice_cabinet_adjusted_setpoint = TypedString('ice_cabinet_adjusted_setpoint', 'ice_cabinet_adjusted_setpoint'),
      shift_offset_calculator = TypedString('shift_offset_calculator', 'shift_offset_calculator')
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(adjusted_setpoint, generate_config())
  end)

  it('should constrain all arguments', function()
    should_fail_with('fresh_food_adjusted_setpoint must be a typed string with type fresh_food_adjusted_setpoint, but is a number', function()
      adjusted_setpoint(generate_config({
        fresh_food_adjusted_setpoint = -1
      }))
    end)

    should_fail_with('freezer_adjusted_setpoint must be a typed string with type freezer_adjusted_setpoint, but is a number', function()
      adjusted_setpoint(generate_config({
        freezer_adjusted_setpoint = -1
      }))
    end)

    should_fail_with('convertible_compartment_adjusted_setpoint must be a typed string with type convertible_compartment_adjusted_setpoint, but is a number', function()
      adjusted_setpoint(generate_config({
        convertible_compartment_adjusted_setpoint = -1
      }))
    end)

    should_fail_with('deli_pan_adjusted_setpoint must be a typed string with type deli_pan_adjusted_setpoint, but is a number', function()
      adjusted_setpoint(generate_config({
        deli_pan_adjusted_setpoint = -1
      }))
    end)

    should_fail_with('ice_cabinet_adjusted_setpoint must be a typed string with type ice_cabinet_adjusted_setpoint, but is a number', function()
      adjusted_setpoint(generate_config({
        ice_cabinet_adjusted_setpoint = -1
      }))
    end)

    should_fail_with('shift_offset_calculator must be a typed string with type shift_offset_calculator, but is a number', function()
      adjusted_setpoint(generate_config({
        shift_offset_calculator = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type adjusted_setpoint', function()
    local expected = remove_whitespace([[
      structure(
        pointer(fresh_food_adjusted_setpoint),
        pointer(freezer_adjusted_setpoint),
        pointer(convertible_compartment_adjusted_setpoint),
        pointer(deli_pan_adjusted_setpoint),
        pointer(ice_cabinet_adjusted_setpoint),
        pointer(shift_offset_calculator)
      )
    ]])

    local actual = adjusted_setpoint(generate_config())

    assert(actual.is_of_type('adjusted_setpoint'))
    assert.equals(expected, remove_whitespace(tostring(actual)))
  end)
end)
