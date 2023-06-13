local ConvertibleCompartmentAdjustedSetpoint = require 'Setpoints/ConvertibleCompartmentAdjustedSetpoint'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('convertible_compartment_adjusted_setpoint', function()
  local convertible_compartment_adjusted_setpoint = ConvertibleCompartmentAdjustedSetpoint(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      cross_ambient_offset_as_fresh_food = TypedString('cross_ambient_offset', 'cross_ambient_offset'),
      cross_ambient_offset_as_freezer = TypedString('cross_ambient_offset', 'cross_ambient_offset'),
      fixed_setpoint_offset = TypedString('fixed_setpoint_offset', 'fixed_setpoint_offset'),
      shift_offset = TypedString('shift_offset', 'shift_offset')
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(convertible_compartment_adjusted_setpoint, generate_config())
  end)

  it('should constrain all arguments', function()
    should_fail_with('cross_ambient_offset_as_fresh_food must be a typed string with type cross_ambient_offset, but is a number', function()
      convertible_compartment_adjusted_setpoint(generate_config({
        cross_ambient_offset_as_fresh_food = -1
      }))
    end)

    should_fail_with('cross_ambient_offset_as_freezer must be a typed string with type cross_ambient_offset, but is a number', function()
      convertible_compartment_adjusted_setpoint(generate_config({
        cross_ambient_offset_as_freezer = -1
      }))
    end)

    should_fail_with('fixed_setpoint_offset must be a typed string with type fixed_setpoint_offset, but is a number', function()
      convertible_compartment_adjusted_setpoint(generate_config({
        fixed_setpoint_offset = -1
      }))
    end)

    should_fail_with('shift_offset must be a typed string with type shift_offset, but is a number', function()
      convertible_compartment_adjusted_setpoint(generate_config({
        shift_offset = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for convertible compartment adjusted setpoint', function()
    local expected = remove_whitespace([[
      structure(
        pointer(cross_ambient_offset),
        pointer(cross_ambient_offset),
        pointer(fixed_setpoint_offset),
        pointer(shift_offset)
    )
    ]])

    local actual = convertible_compartment_adjusted_setpoint(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('convertible_compartment_adjusted_setpoint'))
  end)

  it('should memoize', function()
    should_memoize_calls(convertible_compartment_adjusted_setpoint, generate_config())
  end)
end)
