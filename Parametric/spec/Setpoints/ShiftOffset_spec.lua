local ShiftOffset = require 'Setpoints/ShiftOffset'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('ShiftOffset', function()
  local shift_offset = ShiftOffset(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      max_shift_in_degfx100 = 200,
      min_shift_in_degfx100 = -200,
      upper_adjustment_limit_in_degfx100 = 1000,
      lower_adjustment_limit_in_degfx100 = 1000,
      post_defrost_shift_in_degfx100 = 20
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(shift_offset, generate_config())
  end)

  it('should assert if max_shift_in_degfx100 is not in range', function()
    should_fail_with('max_shift_in_degfx100=32768 must be in [-32768, 32767]', function()
      shift_offset(generate_config({
        max_shift_in_degfx100 = 32768
      }))
    end)
  end)

  it('should assert if min_shift_in_degfx100 is not in range', function()
    should_fail_with('min_shift_in_degfx100=-32769 must be in [-32768, 32767]', function()
      shift_offset(generate_config({
        min_shift_in_degfx100 = -32769
      }))
    end)
  end)

  it('should assert if upper_adjustment_limit_in_degfx100 is not in range', function()
    should_fail_with('upper_adjustment_limit_in_degfx100=32768 must be in [-32768, 32767]', function()
      shift_offset(generate_config({
        upper_adjustment_limit_in_degfx100 = 32768
      }))
    end)
  end)

  it('should assert if lower_adjustment_limit_in_degfx100 is not in range', function()
    should_fail_with('lower_adjustment_limit_in_degfx100=32768 must be in [-32768, 32767]', function()
      shift_offset(generate_config({
        lower_adjustment_limit_in_degfx100 = 32768
      }))
    end)
  end)

  it('should assert if post_defrost_shift_in_degfx100 is not in range', function()
    should_fail_with('post_defrost_shift_in_degfx100=-201 must be in [-200, 200]', function()
      shift_offset(generate_config({
        min_shift_in_degfx100 = -200,
        max_shift_in_degfx100 = 200,
        post_defrost_shift_in_degfx100 = -201
      }))
    end)

    should_fail_with('post_defrost_shift_in_degfx100=201 must be in [-200, 200]', function()
      shift_offset(generate_config({
        min_shift_in_degfx100 = -200,
        max_shift_in_degfx100 = 200,
        post_defrost_shift_in_degfx100 = 201
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for shift offset', function()
    local expected = remove_whitespace([[
      structure(
        i16(200),
        i16(-200),
        i16(1000),
        i16(1000),
        i16(20)
    )
    ]])

    local actual = shift_offset({
      max_shift_in_degfx100 = 200,
      min_shift_in_degfx100 = -200,
      upper_adjustment_limit_in_degfx100 = 1000,
      lower_adjustment_limit_in_degfx100 = 1000,
      post_defrost_shift_in_degfx100 = 20
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('shift_offset'))
  end)

  it('should memoize', function()
    should_memoize_calls(shift_offset, generate_config())
  end)
end)
