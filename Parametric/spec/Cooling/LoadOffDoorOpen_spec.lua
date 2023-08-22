local LoadOffDoorOpen = require 'Cooling/LoadOffDoorOpen'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString
local doors_enum = require 'Cooling.doors_enum'

describe('LoadOffDoorOpen', function()
  local load_off_door_open = LoadOffDoorOpen(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
    }, overrides or {})
  end

  it('should require all arguments including times when enabled', function()
    should_require_args(load_off_door_open, generate_config({
      enable = true,
      doors = { 'right_side_fresh_food_door', 'left_side_freezer_door' },
      timeout_in_seconds = 23,
      restart_delay_in_seconds = 65
    }))
  end)

  it('should only require enable parameter when disabled', function()
    should_require_args(load_off_door_open, generate_config({
      enable = false
    }))
  end)

  it('should assert if enable is not a bool', function()
    should_fail_with('enable must be of type boolean but is of type number', function()
      load_off_door_open(generate_config({
        enable = -1
      }))
    end)
  end)

  it('should require that doors array has >0 elements', function()
    should_fail_with("#doors=0 must be >0", function()
      load_off_door_open(generate_config({
        enable = true,
        doors = { },
        timeout_in_seconds = 3,
        restart_delay_in_seconds = 5
      }))
    end)
  end)

  it('should require that doors are in doors_enum', function()
    should_fail_with("doors[1]='not a valid door' must be in the set { 'left_side_fresh_food_door', 'right_side_fresh_food_door', 'left_side_freezer_door', 'right_side_freezer_door', 'convertible_compartment_door', 'door_in_door' }", function()
      load_off_door_open(generate_config({
        enable = true,
        doors = { 'not a valid door' },
        timeout_in_seconds = 3,
        restart_delay_in_seconds = 5
      }))
    end)

    should_fail_with("doors[2]='not a valid door' must be in the set { 'left_side_fresh_food_door', 'right_side_fresh_food_door', 'left_side_freezer_door', 'right_side_freezer_door', 'convertible_compartment_door', 'door_in_door' }", function()
      load_off_door_open(generate_config({
        enable = true,
        doors = { 'left_side_freezer_door', 'not a valid door'},
        timeout_in_seconds = 3,
        restart_delay_in_seconds = 5
      }))
    end)

    should_fail_with("doors[2]='not a valid door' must be in the set { 'left_side_fresh_food_door', 'right_side_fresh_food_door', 'left_side_freezer_door', 'right_side_freezer_door', 'convertible_compartment_door', 'door_in_door' }", function()
      load_off_door_open(generate_config({
        enable = true,
        doors = { 'left_side_freezer_door', 'not a valid door', 'different invalid door'},
        timeout_in_seconds = 3,
        restart_delay_in_seconds = 5
      }))
    end)
  end)

  it('should assert if timeout_in_seconds is not in range', function()
    should_fail_with('timeout_in_seconds=-1 must be in [0, 255]', function()
      load_off_door_open(generate_config({
        enable = true,
        doors = { 'right_side_fresh_food_door', 'left_side_freezer_door' },
        timeout_in_seconds = -1,
        restart_delay_in_seconds = 10
      }))
    end)
  end)

  it('should assert if restart_delay_in_seconds is not in range', function()
    should_fail_with('restart_delay_in_seconds=-1 must be in [0, 255]', function()
      load_off_door_open(generate_config({
        enable = true,
        doors = { 'right_side_fresh_food_door', 'left_side_freezer_door' },
        timeout_in_seconds = 3,
        restart_delay_in_seconds = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for load off door open when enabled', function()
    local expected = remove_whitespace([[
      structure(
        bool(true),
        u8(6),
        u8(23),
        u8(65)
      )
    ]])

    local actual = load_off_door_open({
      enable = true,
      doors = { 'right_side_fresh_food_door', 'left_side_freezer_door' },
      timeout_in_seconds = 23,
      restart_delay_in_seconds = 65
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('load_off_door_open'))
  end)

  it('should generate a typed string with the correct data and type for load off door open when disabled', function()
    local expected = remove_whitespace([[
      structure(
        bool(false),
        u8(0),
        u8(0),
        u8(0)
      )
    ]])

    local actual = load_off_door_open({
      enable = false
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('load_off_door_open'))
  end)

  it('should memoize when enabled', function()
    should_memoize_calls(load_off_door_open, generate_config({
      enable = true,
      doors = { 'right_side_fresh_food_door', 'left_side_freezer_door' },
      timeout_in_seconds = 23,
      restart_delay_in_seconds = 65
    }))
  end)

  it('should memoize when disabled', function()
    should_memoize_calls(load_off_door_open, generate_config({
      enable = false
    }))
  end)
end)
