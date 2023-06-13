local SetpointOffset = require 'Setpoints/SetpointOffset'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('setpoint_offset', function()
  local setpoint_offset = SetpointOffset(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      cold_setpoint_offset_in_degfx100 = -10,
      mid_setpoint_offset_in_degfx100 = 10,
      warm_setpoint_offset_in_degfx100 = 30
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(setpoint_offset, generate_config())
  end)

  it('should assert if cold_setpoint_offset_in_degfx100 is not in range', function()
    should_fail_with('cold_setpoint_offset_in_degfx100=32768 must be in [-32768, 32767]', function()
      setpoint_offset(generate_config({
        cold_setpoint_offset_in_degfx100 = 32768
      }))
    end)
  end)

  it('should assert if mid_setpoint_offset_in_degfx100 is not in range', function()
    should_fail_with('mid_setpoint_offset_in_degfx100=32768 must be in [-32768, 32767]', function()
      setpoint_offset(generate_config({
        mid_setpoint_offset_in_degfx100 = 32768
      }))
    end)
  end)

  it('should assert if warm_setpoint_offset_in_degfx100 is not in range', function()
    should_fail_with('warm_setpoint_offset_in_degfx100=32768 must be in [-32768, 32767]', function()
      setpoint_offset(generate_config({
        warm_setpoint_offset_in_degfx100 = 32768
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for setpoint offset', function()
    local expected = remove_whitespace([[
      structure(
        i16(-10),
        i16(10),
        i16(30)
    )
    ]])

    local actual = setpoint_offset({
      cold_setpoint_offset_in_degfx100 = -10,
      mid_setpoint_offset_in_degfx100 = 10,
      warm_setpoint_offset_in_degfx100 = 30
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('setpoint_offset'))
  end)

  it('should memoize', function()
    should_memoize_calls(setpoint_offset, generate_config())
  end)
end)
