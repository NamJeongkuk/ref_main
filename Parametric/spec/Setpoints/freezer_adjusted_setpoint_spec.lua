local FreezerAdjustedSetpoint = require 'Setpoints/FreezerAdjustedSetpoint'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('freezer_adjusted_setpoint', function()
  local freezer_adjusted_setpoint = FreezerAdjustedSetpoint(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      freezer_offset_in_degfx100 = -10,
      cross_ambient_offset = TypedString('cross_ambient_offset', 'cross_ambient_offset'),
      shift_offset = TypedString('shift_offset', 'shift_offset')
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(freezer_adjusted_setpoint, generate_config())
  end)

  it('should constrain all arguments', function()
    should_fail_with('cross_ambient_offset must be a typed string with type cross_ambient_offset, but is a number', function()
      freezer_adjusted_setpoint(generate_config({
        cross_ambient_offset = -1
      }))
    end)

    should_fail_with('shift_offset must be a typed string with type shift_offset, but is a number', function()
      freezer_adjusted_setpoint(generate_config({
        shift_offset = -1
      }))
    end)
  end)

  it('should assert if freezer_offset_in_degfx100 is not in range', function()
    should_fail_with('freezer_offset_in_degfx100=32768 must be in [-32768, 32767]', function()
      freezer_adjusted_setpoint(generate_config({
        freezer_offset_in_degfx100 = 32768
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for freezer adjusted setpoint', function()
    local expected = remove_whitespace([[
      structure(
        i16(-10),
        pointer(cross_ambient_offset),
        pointer(shift_offset)
    )
    ]])

    local actual = freezer_adjusted_setpoint(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('freezer_adjusted_setpoint'))
  end)

  it('should memoize', function()
    should_memoize_calls(freezer_adjusted_setpoint, generate_config())
  end)
end)
