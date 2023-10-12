local CabinetOffset = require 'Setpoints/CabinetOffset'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('CabinetOffset', function()
  local cabinet_offset = CabinetOffset(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      fresh_food_offset_in_degfx100 = 10,
      freezer_offset_in_degfx100 = -10,
      ice_box_offset_in_degfx100 = -100
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(cabinet_offset, generate_config())
  end)

  it('should assert if fresh_food_offset_in_degfx100 is not in range', function()
    should_fail_with('fresh_food_offset_in_degfx100=32768 must be in [-32768, 32767]', function()
      cabinet_offset(generate_config({
        fresh_food_offset_in_degfx100 = 32768
      }))
    end)
  end)

  it('should assert if freezer_offset_in_degfx100 is not in range', function()
    should_fail_with('freezer_offset_in_degfx100=32768 must be in [-32768, 32767]', function()
      cabinet_offset(generate_config({
        freezer_offset_in_degfx100 = 32768
      }))
    end)
  end)

  it('should assert if ice_box_offset_in_degfx100 is not in range', function()
    should_fail_with('ice_box_offset_in_degfx100=32768 must be in [-32768, 32767]', function()
      cabinet_offset(generate_config({
        ice_box_offset_in_degfx100 = 32768
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for cabinet offset', function()
    local expected = remove_whitespace([[
      structure(
        i16(10),
        i16(-10),
        i16(-100)
      )
    ]])

    local actual = cabinet_offset({
      fresh_food_offset_in_degfx100 = 10,
      freezer_offset_in_degfx100 = -10,
      ice_box_offset_in_degfx100 = -100
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('cabinet_offset'))
  end)

  it('should memoize', function()
    should_memoize_calls(cabinet_offset, generate_config())
  end)
end)
