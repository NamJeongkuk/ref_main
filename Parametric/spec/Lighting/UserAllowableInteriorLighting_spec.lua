local UserAllowableInteriorLighting = require 'Lighting/UserAllowableInteriorLighting'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_require_args = require 'lua-common'.utilities.should_require_args
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local TypedString = require 'lua-common'.util.TypedString

describe('UserAllowableInteriorLighting', function()
  local user_allowable_interior_lighting = UserAllowableInteriorLighting(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      user_allowable_0_interior_lighting_bitmap = TypedString('user_allowable_interior_lighting_bitmap', 'user_allowable_interior_lighting_bitmap'),
      user_allowable_1_interior_lighting_bitmap = TypedString('user_allowable_interior_lighting_bitmap', 'user_allowable_interior_lighting_bitmap'),
      user_allowable_2_interior_lighting_bitmap = TypedString('user_allowable_interior_lighting_bitmap', 'user_allowable_interior_lighting_bitmap')
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(user_allowable_interior_lighting, generate_config())
  end)

  it('should constrain all arguments', function()
    should_fail_with('user_allowable_0_interior_lighting_bitmap must be a typed string with type user_allowable_interior_lighting_bitmap, but is a number', function()
      user_allowable_interior_lighting(generate_config({
        user_allowable_0_interior_lighting_bitmap = -1
      }))
    end)

    should_fail_with('user_allowable_1_interior_lighting_bitmap must be a typed string with type user_allowable_interior_lighting_bitmap, but is a number', function()
      user_allowable_interior_lighting(generate_config({
        user_allowable_1_interior_lighting_bitmap = -1
      }))
    end)

    should_fail_with('user_allowable_2_interior_lighting_bitmap must be a typed string with type user_allowable_interior_lighting_bitmap, but is a number', function()
      user_allowable_interior_lighting(generate_config({
        user_allowable_2_interior_lighting_bitmap = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type user_allowable_interior_lighting', function()
    local expected = remove_whitespace([[
      structure(
        user_allowable_interior_lighting_bitmap,
        user_allowable_interior_lighting_bitmap,
        user_allowable_interior_lighting_bitmap
      )
    ]])

    local actual = user_allowable_interior_lighting(generate_config())

    assert(actual.is_of_type('user_allowable_interior_lighting'))
    assert.equals(expected, remove_whitespace(tostring(actual)))
  end)
end)
