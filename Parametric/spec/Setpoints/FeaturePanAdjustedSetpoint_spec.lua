local FeaturePanAdjustedSetpoint = require 'Setpoints/FeaturePanAdjustedSetpoint'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('FeaturePanAdjustedSetpoint', function()
  local feature_pan_adjusted_setpoint = FeaturePanAdjustedSetpoint(core_mock)

  local function generate_config_as_convertible_compartment(overrides)
    return require 'lua-common'.table.merge({
      setpoint_offset = TypedString('feature_pan_setpoint_offset', 'feature_pan_setpoint_offset'),
      shift_offset = TypedString('shift_offset', 'shift_offset'),
      cross_ambient_offset_as_fresh_food = TypedString('cross_ambient_offset', 'cross_ambient_offset'),
      cross_ambient_offset_as_freezer = TypedString('cross_ambient_offset', 'cross_ambient_offset')
    }, overrides or {})
  end

  local function generate_config_as_deli_pan(overrides)
    return require 'lua-common'.table.merge({
      setpoint_offset = TypedString('feature_pan_setpoint_offset', 'feature_pan_setpoint_offset'),
      shift_offset = TypedString('shift_offset', 'shift_offset')
    }, overrides or {})
  end

  it('should require fixed setpoint and shift offset arguments', function()
    should_require_args(feature_pan_adjusted_setpoint, generate_config_as_deli_pan())
  end)

  it('should constrain all arguments', function()
    should_fail_with('cross_ambient_offset_as_fresh_food must be a typed string with type cross_ambient_offset, but is a number', function()
      feature_pan_adjusted_setpoint(generate_config_as_convertible_compartment({
        cross_ambient_offset_as_fresh_food = -1
      }))
    end)

    should_fail_with('cross_ambient_offset_as_freezer must be a typed string with type cross_ambient_offset, but is a number', function()
      feature_pan_adjusted_setpoint(generate_config_as_convertible_compartment({
        cross_ambient_offset_as_freezer = -1
      }))
    end)

    should_fail_with('setpoint_offset must be a typed string with type feature_pan_setpoint_offset, but is a number', function()
      feature_pan_adjusted_setpoint(generate_config_as_convertible_compartment({
        setpoint_offset = -1
      }))
    end)

    should_fail_with('shift_offset must be a typed string with type shift_offset, but is a number', function()
      feature_pan_adjusted_setpoint(generate_config_as_convertible_compartment({
        shift_offset = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for convertible compartment adjusted setpoint', function()
    local expected = remove_whitespace([[
      structure(
        pointer(feature_pan_setpoint_offset),
        pointer(shift_offset),
        pointer(cross_ambient_offset),
        pointer(cross_ambient_offset)
    )
    ]])

    local actual = feature_pan_adjusted_setpoint(generate_config_as_convertible_compartment())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('feature_pan_adjusted_setpoint'))
  end)

  it('should generate a typed string with the correct data and type for deli pan adjusted setpoint', function()
    local expected = remove_whitespace([[
      structure(
        pointer(feature_pan_setpoint_offset),
        pointer(shift_offset),
        pointer(nil),
        pointer(nil)
    )
    ]])

    local actual = feature_pan_adjusted_setpoint(generate_config_as_deli_pan())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('feature_pan_adjusted_setpoint'))
  end)

  it('should memoize', function()
    should_memoize_calls(feature_pan_adjusted_setpoint, generate_config_as_convertible_compartment())
    should_memoize_calls(feature_pan_adjusted_setpoint, generate_config_as_deli_pan())
  end)
end)
