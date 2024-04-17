local FeaturePanSetpointOffset = require 'Setpoints/FeaturePanSetpointOffset'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('FeaturePanSetpointOffset', function()
  local feature_pan_setpoint_offset = FeaturePanSetpointOffset(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      mode1_offset_in_degfx100 = -30,
      mode2_offset_in_degfx100 = -20,
      mode3_offset_in_degfx100 = -10,
      mode4_offset_in_degfx100 = 0,
      mode5_offset_in_degfx100 = 10,
      mode6_offset_in_degfx100 = 20,
      mode7_offset_in_degfx100 = 30
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(feature_pan_setpoint_offset, generate_config())
  end)

  it('should assert if mode1_offset_in_degfx100 is not in range', function()
    should_fail_with('mode1_offset_in_degfx100=32768 must be in [-32768, 32767]', function()
      feature_pan_setpoint_offset(generate_config({
        mode1_offset_in_degfx100 = 32768
      }))
    end)
  end)

  it('should assert if mode2_offset_in_degfx100 is not in range', function()
    should_fail_with('mode2_offset_in_degfx100=32768 must be in [-32768, 32767]', function()
      feature_pan_setpoint_offset(generate_config({
        mode2_offset_in_degfx100 = 32768
      }))
    end)
  end)

  it('should assert if mode3_offset_in_degfx100 is not in range', function()
    should_fail_with('mode3_offset_in_degfx100=32768 must be in [-32768, 32767]', function()
      feature_pan_setpoint_offset(generate_config({
        mode3_offset_in_degfx100 = 32768
      }))
    end)
  end)

  it('should assert if mode4_offset_in_degfx100 is not in range', function()
    should_fail_with('mode4_offset_in_degfx100=32768 must be in [-32768, 32767]', function()
      feature_pan_setpoint_offset(generate_config({
        mode4_offset_in_degfx100 = 32768
      }))
    end)
  end)

  it('should assert if mode5_offset_in_degfx100 is not in range', function()
    should_fail_with('mode5_offset_in_degfx100=32768 must be in [-32768, 32767]', function()
      feature_pan_setpoint_offset(generate_config({
        mode5_offset_in_degfx100 = 32768
      }))
    end)
  end)

  it('should assert if mode6_offset_in_degfx100 is not in range', function()
    should_fail_with('mode6_offset_in_degfx100=32768 must be in [-32768, 32767]', function()
      feature_pan_setpoint_offset(generate_config({
        mode6_offset_in_degfx100 = 32768
      }))
    end)
  end)

  it('should assert if mode7_offset_in_degfx100 is not in range', function()
    should_fail_with('mode7_offset_in_degfx100=32768 must be in [-32768, 32767]', function()
      feature_pan_setpoint_offset(generate_config({
        mode7_offset_in_degfx100 = 32768
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for feature pan setpoint offset', function()
    local expected = remove_whitespace([[
      structure(
        i16(-30),
        i16(-20),
        i16(-10),
        i16(0),
        i16(10),
        i16(20),
        i16(30)
    )
    ]])

    local actual = feature_pan_setpoint_offset({
      mode1_offset_in_degfx100 = -30,
      mode2_offset_in_degfx100 = -20,
      mode3_offset_in_degfx100 = -10,
      mode4_offset_in_degfx100 = 0,
      mode5_offset_in_degfx100 = 10,
      mode6_offset_in_degfx100 = 20,
      mode7_offset_in_degfx100 = 30
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('feature_pan_setpoint_offset'))
  end)

  it('should memoize', function()
    should_memoize_calls(feature_pan_setpoint_offset, generate_config())
  end)
end)
