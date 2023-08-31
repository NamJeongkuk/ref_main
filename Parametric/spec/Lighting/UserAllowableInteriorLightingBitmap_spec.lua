local UserAllowableInteriorLightingBitmap = require 'Lighting/UserAllowableInteriorLightingBitmap'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString
local interior_light_enum = require 'Lighting.interior_light_enum'

describe('UserAllowableInteriorLightingBitmap', function()
  local user_allowable_interior_lighting_bitmap = UserAllowableInteriorLightingBitmap(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(user_allowable_interior_lighting_bitmap, generate_config({
      lights = { 'fresh_food_back_light', 'fresh_food_top_light' }
    }))
  end)

  it('should require that lights are in interior_light_enum', function()
    should_fail_with("lights[1]='not a light' must be in the set { 'fresh_food_back_light', 'fresh_food_top_light', 'freezer_back_light', 'freezer_top_light' }", function()
      user_allowable_interior_lighting_bitmap(generate_config({
        lights = { 'not a light' }
      }))
    end)

    should_fail_with("lights[2]='not a light' must be in the set { 'fresh_food_back_light', 'fresh_food_top_light', 'freezer_back_light', 'freezer_top_light' }", function()
      user_allowable_interior_lighting_bitmap(generate_config({
        lights = { 'fresh_food_back_light', 'not a light' }
      }))
    end)

    should_fail_with("lights[2]='not a light' must be in the set { 'fresh_food_back_light', 'fresh_food_top_light', 'freezer_back_light', 'freezer_top_light' }", function()
      user_allowable_interior_lighting_bitmap(generate_config({
        lights = { 'fresh_food_back_light', 'not a light', 'definitely not a light' }
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type', function()
    local expected = remove_whitespace([[
      structure(
        u8(3)
      )
    ]])

    local actual = user_allowable_interior_lighting_bitmap({
      lights = { 'fresh_food_back_light', 'fresh_food_top_light' }
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('user_allowable_interior_lighting_bitmap'))
  end)

  it('should memoize', function()
    should_memoize_calls(user_allowable_interior_lighting_bitmap, generate_config({
      lights = { 'fresh_food_back_light', 'fresh_food_top_light' }
    }))
  end)
end)
