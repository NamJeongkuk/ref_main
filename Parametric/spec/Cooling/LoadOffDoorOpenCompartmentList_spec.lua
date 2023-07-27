local LoadOffDoorOpenCompartmentList = require 'Cooling/LoadOffDoorOpenCompartmentList'
local LoadOffDoorOpen = require 'Cooling/LoadOffDoorOpen'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.utilities.TypedString

describe('LoadOffDoorOpenCompartmentList', function()
  local load_off_door_open_compartment_list = LoadOffDoorOpenCompartmentList(core_mock)
  local load_off_door_open = LoadOffDoorOpen(core_mock)

  local some_fresh_food_load_off_door_open = load_off_door_open({
    enable = true,
    timeout_in_seconds = 32,
    restart_delay_in_seconds = 89
  })

  local some_freezer_load_off_door_open = load_off_door_open({
    enable = true,
    timeout_in_seconds = 48,
    restart_delay_in_seconds = 62
  })

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      fresh_food_compartment = some_fresh_food_load_off_door_open,
      freezer_compartment = some_freezer_load_off_door_open
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(load_off_door_open_compartment_list, generate_config())
  end)

  it('should assert if fresh_food_compartment is not of type string load_off_door_open', function()
    should_fail_with('fresh_food_compartment must be a typed string with type load_off_door_open, but is a number', function()
      load_off_door_open_compartment_list(generate_config({
        fresh_food_compartment = 2
      }))
    end)
  end)

  it('should assert if freezer_compartment is not of type string load_off_door_open', function()
    should_fail_with('freezer_compartment must be a typed string with type load_off_door_open, but is a number', function()
      load_off_door_open_compartment_list(generate_config({
        freezer_compartment = 2
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type load_off_door_open_compartment_list', function()
    local expected = remove_whitespace([[
      structure(
        structure(
          bool(true),
          u8(32),
          u8(89)
        ),
        structure(
          bool(true),
          u8(48),
          u8(62)
        )
      )
    ]])

    local actual = load_off_door_open_compartment_list({
      fresh_food_compartment = some_fresh_food_load_off_door_open,
      freezer_compartment = some_freezer_load_off_door_open
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('load_off_door_open_compartment_list'))
  end)

  it('should memoize', function()
    should_memoize_calls(load_off_door_open_compartment_list, generate_config())
  end)
end)
