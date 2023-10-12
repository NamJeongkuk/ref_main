local DoorInhibitDispenseTable = require 'Dispenser/DoorInhibitDispenseTable'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('DoorInhibitDispenseTable', function()
  local doorInhibitDispenseTable = DoorInhibitDispenseTable(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      left_hand_fresh_food = true,
      right_hand_fresh_food = true,
      left_hand_freezer = true,
      right_hand_freezer = true,
      convertible_compartment = true,
      door_in_door = true
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(doorInhibitDispenseTable, generate_config())
  end)

  it('should assert if left_hand_fresh_food is not a boolean', function()
    should_fail_with('left_hand_fresh_food must be of type boolean but is of type number', function()
      doorInhibitDispenseTable(generate_config({
        left_hand_fresh_food = -1
      }))
    end)
  end)

  it('should assert if right_hand_fresh_food is not a boolean', function()
    should_fail_with('right_hand_fresh_food must be of type boolean but is of type number', function()
      doorInhibitDispenseTable(generate_config({
        right_hand_fresh_food = -1
      }))
    end)
  end)

  it('should assert if left_hand_freezer is not a boolean', function()
    should_fail_with('left_hand_freezer must be of type boolean but is of type number', function()
      doorInhibitDispenseTable(generate_config({
        left_hand_freezer = -1
      }))
    end)
  end)

  it('should assert if right_hand_freezer is not a boolean', function()
    should_fail_with('right_hand_freezer must be of type boolean but is of type number', function()
      doorInhibitDispenseTable(generate_config({
        right_hand_freezer = -1
      }))
    end)
  end)

  it('should assert if convertible_compartment is not a boolean', function()
    should_fail_with('convertible_compartment must be of type boolean but is of type number', function()
      doorInhibitDispenseTable(generate_config({
        convertible_compartment = -1
      }))
    end)
  end)

  it('should assert if door_in_door is not a boolean', function()
    should_fail_with('door_in_door must be of type boolean but is of type number', function()
      doorInhibitDispenseTable(generate_config({
        door_in_door = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type door_inhibit_dispense_table', function()
    local expected = remove_whitespace([[
      structure(
        bool(true),
        bool(true),
        bool(true),
        bool(true),
        bool(true),
        bool(true)
      )
    ]])

    local actual = doorInhibitDispenseTable({
      left_hand_fresh_food = true,
      right_hand_fresh_food = true,
      left_hand_freezer = true,
      right_hand_freezer = true,
      convertible_compartment = true,
      door_in_door = true
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('door_inhibit_dispense_table'))
  end)

  it('should memoize', function()
    should_memoize_calls(doorInhibitDispenseTable, generate_config())
  end)
end)
