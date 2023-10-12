local Dispenser = require 'Dispenser/Dispenser'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('Dispenser', function()
  local dispenser = Dispenser(core_mock)
  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      maximum_dispensed_ouncesx100 = 12800,
      maximum_dispense_time_in_seconds = 420,
      door_inhibit_water_dispense_table = TypedString('door_inhibit_dispense_table', 'door_inhibit_water_dispense_table'),
      door_inhibit_ice_dispense_table = TypedString('door_inhibit_dispense_table', 'door_inhibit_ice_dispense_table')
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(dispenser, generate_config())
  end)

  it('should assert if maximum_dispensed_ouncesx100 is not in range', function()
    should_fail_with('maximum_dispensed_ouncesx100=-1 must be in [0, 65535]', function()
      dispenser(generate_config({
        maximum_dispensed_ouncesx100 = -1
      }))
    end)
  end)

  it('should assert if maximum_dispense_time_in_seconds is not in range', function()
    should_fail_with('maximum_dispense_time_in_seconds=-1 must be in [0, 65535]', function()
      dispenser(generate_config({
        maximum_dispense_time_in_seconds = -1
      }))
    end)
  end)

  it('should assert if door_inhibit_water_dispense_table is not a string', function()
    should_fail_with('door_inhibit_water_dispense_table must be a typed string with type door_inhibit_dispense_table, but is a number', function()
      dispenser(generate_config({
        door_inhibit_water_dispense_table = 1
      }))
    end)
  end)

  it('should assert if door_inhibit_ice_dispense_table is not a string', function()
    should_fail_with('door_inhibit_ice_dispense_table must be a typed string with type door_inhibit_dispense_table, but is a number', function()
      dispenser(generate_config({
        door_inhibit_ice_dispense_table = 1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type dispenser', function()
    local expected = remove_whitespace([[
      structure(
        u16(12800),
        u16(420),
        pointer(door_inhibit_water_dispense_table),
        pointer(door_inhibit_ice_dispense_table)
      )
      ]])

    local actual = dispenser({
      maximum_dispensed_ouncesx100 = 12800,
      maximum_dispense_time_in_seconds = 420,
      door_inhibit_water_dispense_table = TypedString('door_inhibit_dispense_table', 'door_inhibit_water_dispense_table'),
      door_inhibit_ice_dispense_table = TypedString('door_inhibit_dispense_table', 'door_inhibit_ice_dispense_table')
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('dispenser'))
  end)

  it('should memoize', function()
    should_memoize_calls(dispenser, generate_config())
  end)
end)
