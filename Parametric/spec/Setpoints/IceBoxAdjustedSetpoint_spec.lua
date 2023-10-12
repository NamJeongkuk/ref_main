local IceBoxAdjustedSetpoint = require 'Setpoints/IceBoxAdjustedSetpoint'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('IceBoxAdjustedSetpoint', function()
  local ice_box_adjusted_setpoint = IceBoxAdjustedSetpoint(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      ice_formation_offset_in_degfx100 = -200,
      shift_offset = TypedString('shift_offset', 'shift_offset')
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(ice_box_adjusted_setpoint, generate_config())
  end)

  it('should constrain all arguments', function()
    should_fail_with('shift_offset must be a typed string with type shift_offset, but is a number', function()
      ice_box_adjusted_setpoint(generate_config({
        shift_offset = -1
      }))
    end)
  end)

  it('should assert if ice_formation_offset_in_degfx100 is not in range', function()
    should_fail_with('ice_formation_offset_in_degfx100=32768 must be in [-32768, 32767]', function()
      ice_box_adjusted_setpoint(generate_config({
        ice_formation_offset_in_degfx100 = 32768
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for ice box adjusted setpoint', function()
    local expected = remove_whitespace([[
      structure(
        i16(-200),
        pointer(shift_offset)
    )
    ]])

    local actual = ice_box_adjusted_setpoint(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('ice_box_adjusted_setpoint'))
  end)

  it('should memoize', function()
    should_memoize_calls(ice_box_adjusted_setpoint, generate_config())
  end)
end)
