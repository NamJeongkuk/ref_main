local Lighting = require 'Lighting/Lighting'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_require_args = require 'lua-common'.utilities.should_require_args
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local TypedString = require 'lua-common'.util.TypedString

describe('Lighting', function()
  local lighting = Lighting(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      maximum_light_duty_cycle_percentage = 100,
      maximum_interior_lights_on_time_in_minutes = 15,
      user_allowable_interior_lighting = TypedString('user_allowable_interior_lighting', 'user_allowable_interior_lighting'),
      fresh_food_backwall = TypedString('door_lighting', 'door_lighting'),
      fresh_food_top_and_side = TypedString('door_lighting', 'door_lighting'),
      freezer_backwall = TypedString('door_lighting', 'door_lighting'),
      freezer_top_and_side = TypedString('door_lighting', 'door_lighting')
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(lighting, generate_config())
  end)

  it('should constrain all arguments', function()
    should_fail_with('maximum_light_duty_cycle_percentage=101 must be in [0, 100]', function()
      lighting(generate_config({
        maximum_light_duty_cycle_percentage = 101
      }))
    end)

    should_fail_with('maximum_interior_lights_on_time_in_minutes=-1 must be in [0, 255]', function()
      lighting(generate_config({
        maximum_interior_lights_on_time_in_minutes = -1
      }))
    end)

    should_fail_with('user_allowable_interior_lighting must be a typed string with type user_allowable_interior_lighting, but is a number', function()
      lighting(generate_config({
        user_allowable_interior_lighting = -1
      }))
    end)

    should_fail_with('fresh_food_backwall must be a typed string with type door_lighting, but is a number', function()
      lighting(generate_config({
        fresh_food_backwall = -1
      }))
    end)

    should_fail_with('fresh_food_top_and_side must be a typed string with type door_lighting, but is a number', function()
      lighting(generate_config({
        fresh_food_top_and_side = -1
      }))
    end)

    should_fail_with('freezer_backwall must be a typed string with type door_lighting, but is a number', function()
      lighting(generate_config({
        freezer_backwall = -1
      }))
    end)

    should_fail_with('freezer_top_and_side must be a typed string with type door_lighting, but is a number', function()
      lighting(generate_config({
        freezer_top_and_side = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type lighting', function()
    local expected = remove_whitespace([[
      structure(
        u8(100),
        u8(15),
        structure(
          pointer(user_allowable_interior_lighting),
          pointer(door_lighting),
          pointer(door_lighting),
          pointer(door_lighting),
          pointer(door_lighting)
        )
      )
    ]])

    local actual = lighting(generate_config())

    assert(actual.is_of_type('lighting'))
    assert.equals(expected, remove_whitespace(tostring(actual)))
  end)
end)
