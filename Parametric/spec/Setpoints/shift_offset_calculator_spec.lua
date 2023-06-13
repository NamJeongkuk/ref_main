local ShiftOffsetCalculator = require 'Setpoints/ShiftOffsetCalculator'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('shift_offset_calculator', function()
  local shift_offset_calculator = ShiftOffsetCalculator(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      long_term_beta = 100,
      long_term_average_update_time_in_minutes = 5,
      shift_update_time_in_minutes = 15
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(shift_offset_calculator, generate_config())
  end)

  it('should assert if long_term_beta is not in range', function()
    should_fail_with('long_term_beta=-1 must be in [0, 255]', function()
      shift_offset_calculator(generate_config({
        long_term_beta = -1
      }))
    end)
  end)

  it('should assert if long_term_average_update_time_in_minutes is not in range', function()
    should_fail_with('long_term_average_update_time_in_minutes=-1 must be in [0, 255]', function()
      shift_offset_calculator(generate_config({
        long_term_average_update_time_in_minutes = -1
      }))
    end)

    should_fail_with('long_term_average_update_time_in_minutes=255 must be <15', function()
      shift_offset_calculator(generate_config({
        long_term_average_update_time_in_minutes = 255,
        shift_update_time_in_minutes = 15
      }))
    end)
  end)

  it('should assert if shift_update_time_in_minutes is not in range', function()
    should_fail_with('shift_update_time_in_minutes=256 must be in [0, 255]', function()
      shift_offset_calculator(generate_config({
        shift_update_time_in_minutes = 256
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for shift offset calculator', function()
    local expected = remove_whitespace([[
      structure(
        u8(100),
        u8(5),
        u8(15)
    )
    ]])

    local actual = shift_offset_calculator(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('shift_offset_calculator'))
  end)

  it('should memoize', function()
    should_memoize_calls(shift_offset_calculator, generate_config())
  end)
end)
