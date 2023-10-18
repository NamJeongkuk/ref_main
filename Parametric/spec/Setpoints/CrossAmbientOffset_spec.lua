local CrossAmbientOffset = require 'Setpoints/CrossAmbientOffset'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('CrossAmbientOffset', function()
  local cross_ambient_offset = CrossAmbientOffset(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      cross_ambient_slope_coefficient = 100,
      minimum_cross_ambient_offset_in_degfx100 = -3000,
      maximum_cross_ambient_offset_in_degfx100 = 1000
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(cross_ambient_offset, generate_config())
  end)

  it('should assert if cross_ambient_slope_coefficient is not in range', function()
    should_fail_with('cross_ambient_slope_coefficient=32768 must be in [-32768, 32767]', function()
      cross_ambient_offset(generate_config({
        cross_ambient_slope_coefficient = 32768
      }))
    end)
  end)

  it('should assert if minimum_cross_ambient_offset_in_degfx100 is not in range', function()
    should_fail_with('minimum_cross_ambient_offset_in_degfx100=32768 must be in [-32768, 32767]', function()
      cross_ambient_offset(generate_config({
        minimum_cross_ambient_offset_in_degfx100 = 32768
      }))
    end)
  end)

  it('should assert if maximum_cross_ambient_offset_in_degfx100 is not in range', function()
    should_fail_with('maximum_cross_ambient_offset_in_degfx100=32768 must be in [-32768, 32767]', function()
      cross_ambient_offset(generate_config({
        maximum_cross_ambient_offset_in_degfx100 = 32768
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for cross ambient offset', function()
    local expected = remove_whitespace([[
      structure(
        i16(1000),
        i16(-3000),
        i16(1000)
    )
    ]])

    local actual = cross_ambient_offset({
      cross_ambient_slope_coefficient = 1000,
      minimum_cross_ambient_offset_in_degfx100 = -3000,
      maximum_cross_ambient_offset_in_degfx100 = 1000
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('cross_ambient_offset'))
  end)

  it('should memoize', function()
    should_memoize_calls(cross_ambient_offset, generate_config())
  end)
end)
