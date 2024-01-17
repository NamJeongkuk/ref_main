local IceMakerSlots = require 'IceMaker/IceMakerSlots'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('IceMakerSlots',  function()
  local ice_maker_slots = IceMakerSlots(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      ice_maker_slot_0 = TypedString('ice_maker_slot', 'ice_maker_slot'),
      ice_maker_slot_1 = TypedString('ice_maker_slot', 'ice_maker_slot'),
      ice_maker_slot_2 = TypedString('ice_maker_slot', 'ice_maker_slot')
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(ice_maker_slots, generate_config())
  end)
  
  it('should assert if ice_maker_slot_0 is not a string', function()
    should_fail_with('ice_maker_slot_0 must be a typed string with type ice_maker_slot', function()
      ice_maker_slots(generate_config({
        ice_maker_slot_0 = 1
      }))
    end)
  end)
  
  it('should assert if ice_maker_slot_1 is not a string', function()
    should_fail_with('ice_maker_slot_1 must be a typed string with type ice_maker_slot', function()
      ice_maker_slots(generate_config({
        ice_maker_slot_1 = 1
      }))
    end)
  end)
  
  it('should assert if ice_maker_slot_2 is not a string', function()
    should_fail_with('ice_maker_slot_2 must be a typed string with type ice_maker_slot', function()
      ice_maker_slots(generate_config({
        ice_maker_slot_2 = 1
      }))
    end)
  end)
  
  it('should generate a typed string with the correct data and type for ice_maker_slots', function()
    local expected = remove_whitespace([[
      structure(
        pointer(ice_maker_slot),
        pointer(ice_maker_slot),
        pointer(ice_maker_slot)
      )
    ]])

    local actual = ice_maker_slots({
      ice_maker_slot_0 = TypedString('ice_maker_slot', 'ice_maker_slot'),
      ice_maker_slot_1 = TypedString('ice_maker_slot', 'ice_maker_slot'),
      ice_maker_slot_2 = TypedString('ice_maker_slot', 'ice_maker_slot'),
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('ice_maker_slots'))
  end)

  it('should_memoize', function()
    should_memoize_calls(ice_maker_slots, generate_config())
  end)
end)
