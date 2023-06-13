local DeliPanAdjustedSetpoint = require 'Setpoints/DeliPanAdjustedSetpoint'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('deli_pan_adjusted_setpoint', function()
  local deli_pan_adjusted_setpoint = DeliPanAdjustedSetpoint(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      fixed_setpoint_offset = TypedString('fixed_setpoint_offset', 'fixed_setpoint_offset'),
      shift_offset = TypedString('shift_offset', 'shift_offset')
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(deli_pan_adjusted_setpoint, generate_config())
  end)

  it('should constrain all arguments', function()
    should_fail_with('fixed_setpoint_offset must be a typed string with type fixed_setpoint_offset, but is a number', function()
      deli_pan_adjusted_setpoint(generate_config({
        fixed_setpoint_offset = -1
      }))
    end)

    should_fail_with('shift_offset must be a typed string with type shift_offset, but is a number', function()
      deli_pan_adjusted_setpoint(generate_config({
        shift_offset = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for deli pan adjusted setpoint', function()
    local expected = remove_whitespace([[
      structure(
        pointer(fixed_setpoint_offset),
        pointer(shift_offset)
    )
    ]])

    local actual = deli_pan_adjusted_setpoint(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('deli_pan_adjusted_setpoint'))
  end)

  it('should memoize', function()
    should_memoize_calls(deli_pan_adjusted_setpoint, generate_config())
  end)
end)
