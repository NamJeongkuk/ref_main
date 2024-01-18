local IceMakerSlot = require 'IceMaker/IceMakerSlot'
local ice_maker_type = require 'IceMaker/IceMakerType'
local ice_maker_location = require 'IceMaker/IceMakerLocation'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('IceMakerSlot', function()
  local ice_maker_slot = IceMakerSlot(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      }, overrides or {})
  end

  it('should require all arguments when enabled', function()
    should_require_args(ice_maker_slot, generate_config({
      enable = true,
      type = ice_maker_type.aluminum_mold,
      location = ice_maker_location.freezer,
      ice_maker_data = TypedString('aluminum_mold_ice_maker', 'aluminum_mold_ice_maker'),
      freezer_ice_rate = TypedString('freezer_ice_rate', 'freezer_ice_rate'),
      non_harvest_fill_tube_heater = TypedString('non_harvest_fill_tube_heater', 'non_harvest_fill_tube_heater')
    }))
  end)

  it('should only require enable parameter when disabled', function()
    should_require_args(ice_maker_slot, generate_config({
      enable = false
    }))
  end)

  it('should assert if enable is not a boolean', function()
    should_fail_with('enable must be of type boolean but is of type number', function()
      ice_maker_slot(generate_config({
        enable = -1
      }))
    end)
  end)

  it('should assert if type is not a ice_maker_type', function()
    should_fail_with('type=not_a_type is not in the ice_maker_type enumeration, allowable values: aluminum_mold, twist_tray', function()
      ice_maker_slot(generate_config({
        enable = true,
        type = 'not_a_type',
        location = ice_maker_location.freezer,
        ice_maker_data = TypedString('aluminum_mold_ice_maker', 'aluminum_mold_ice_maker'),
        freezer_ice_rate = TypedString('freezer_ice_rate', 'freezer_ice_rate'),
        non_harvest_fill_tube_heater = TypedString('non_harvest_fill_tube_heater', 'non_harvest_fill_tube_heater')
      }))
    end)
  end)

  it('should assert if location is not a ice_maker_location', function()
    should_fail_with('location=not_a_location is not in the ice_maker_location enumeration, allowable values: freezer, fresh_food', function()
      ice_maker_slot(generate_config({
        enable = true,
        type = ice_maker_type.aluminum_mold,
        location = 'not_a_location',
        ice_maker_data = TypedString('aluminum_mold_ice_maker', 'aluminum_mold_ice_maker'),
        freezer_ice_rate = TypedString('freezer_ice_rate', 'freezer_ice_rate'),
        non_harvest_fill_tube_heater = TypedString('non_harvest_fill_tube_heater', 'non_harvest_fill_tube_heater')
      }))
    end)
  end)

  it('should assert if ice_maker_data is not a aluminum_mold_ice_maker when type is aluminum_mold', function()
    should_fail_with('ice_maker_data must be a typed string with type aluminum_mold_ice_maker, has type(s) twist_tray_ice_maker', function()
      ice_maker_slot(generate_config({
        enable = true,
        type = ice_maker_type.aluminum_mold,
        location = ice_maker_location.freezer,
        ice_maker_data = TypedString('twist_tray_ice_maker', 'twist_tray_ice_maker'),
        freezer_ice_rate = TypedString('freezer_ice_rate', 'freezer_ice_rate'),
        non_harvest_fill_tube_heater = TypedString('non_harvest_fill_tube_heater', 'non_harvest_fill_tube_heater')
      }))
    end)
  end)

  it('should assert if ice_maker_data is not a twist_tray_ice_maker when type is twist_tray', function()
    should_fail_with('ice_maker_data must be a typed string with type twist_tray_ice_maker, has type(s) aluminum_mold_ice_maker', function()
      ice_maker_slot(generate_config({
        enable = true,
        type = ice_maker_type.twist_tray,
        location = ice_maker_location.freezer,
        ice_maker_data = TypedString('aluminum_mold_ice_maker', 'aluminum_mold_ice_maker'),
        freezer_ice_rate = TypedString('freezer_ice_rate', 'freezer_ice_rate'),
        non_harvest_fill_tube_heater = TypedString('non_harvest_fill_tube_heater', 'non_harvest_fill_tube_heater')
      }))
    end)
  end)

  it('should assert if freezer_ice_rate is not a freezer_ice_rate', function()
    should_fail_with('freezer_ice_rate must be a typed string with type freezer_ice_rate, but is a number', function()
      ice_maker_slot(generate_config({
        enable = true,
        type = ice_maker_type.twist_tray,
        location = ice_maker_location.freezer,
        ice_maker_data = TypedString('twist_tray_ice_maker', 'twist_tray_ice_maker'),
        freezer_ice_rate = 1,
        non_harvest_fill_tube_heater = TypedString('non_harvest_fill_tube_heater', 'non_harvest_fill_tube_heater')
      }))
    end)
  end)

  it('should assert if non_harvest_fill_tube_heater is not a non_harvest_fill_tube_heater', function()
    should_fail_with('non_harvest_fill_tube_heater must be a typed string with type non_harvest_fill_tube_heater, but is a number', function()
      ice_maker_slot(generate_config({
        enable = true,
        type = ice_maker_type.twist_tray,
        location = ice_maker_location.freezer,
        ice_maker_data = TypedString('twist_tray_ice_maker', 'twist_tray_ice_maker'),
        freezer_ice_rate = TypedString('freezer_ice_rate', 'freezer_ice_rate'),
        non_harvest_fill_tube_heater = 1
      }))
    end)
  end)

  it('should generate a typed string with the correct data when disabled', function()
    local expected = remove_whitespace([[
      structure(
        bool(false),
        u8(0),
        u8(0),
        pointer(u8(0)),
        pointer(u8(0)),
        pointer(u8(0))
      )
     ]])

    local actual = ice_maker_slot({
      enable = false
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('ice_maker_slot'))
  end)

  it('should generate a typed string with the correct data when enabled and configured as twist tray', function()
    local expected = remove_whitespace([[
      structure(
        bool(true),
        u8(1),
        u8(0),
        pointer(twist_tray_ice_maker),
        pointer(freezer_ice_rate),
        pointer(non_harvest_fill_tube_heater)
      )
    ]])

    local actual = ice_maker_slot({
      enable = true,
      type = ice_maker_type.twist_tray,
      location = ice_maker_location.freezer,
      ice_maker_data = TypedString('twist_tray_ice_maker', 'twist_tray_ice_maker'),
      freezer_ice_rate = TypedString('freezer_ice_rate', 'freezer_ice_rate'),
      non_harvest_fill_tube_heater = TypedString('non_harvest_fill_tube_heater', 'non_harvest_fill_tube_heater')
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('ice_maker_slot'))
  end)

  it('should generate a typed string with the correct data when enabled and configured as aluminum mold', function()
    local expected = remove_whitespace([[
      structure(
        bool(true),
        u8(0),
        u8(0),
        pointer(aluminum_mold_ice_maker),
        pointer(freezer_ice_rate),
        pointer(non_harvest_fill_tube_heater)
      )
    ]])

    local actual = ice_maker_slot({
      enable = true,
      type = ice_maker_type.aluminum_mold,
      location = ice_maker_location.freezer,
      ice_maker_data = TypedString('aluminum_mold_ice_maker', 'aluminum_mold_ice_maker'),
      freezer_ice_rate = TypedString('freezer_ice_rate', 'freezer_ice_rate'),
      non_harvest_fill_tube_heater = TypedString('non_harvest_fill_tube_heater', 'non_harvest_fill_tube_heater')
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('ice_maker_slot'))
  end)

  it('should memoize', function()
    should_memoize_calls(ice_maker_slot, generate_config({
      enable = true,
      type = ice_maker_type.aluminum_mold,
      location = ice_maker_location.freezer,
      ice_maker_data = TypedString('aluminum_mold_ice_maker', 'aluminum_mold_ice_maker'),
      freezer_ice_rate = TypedString('freezer_ice_rate', 'freezer_ice_rate'),
      non_harvest_fill_tube_heater = TypedString('non_harvest_fill_tube_heater', 'non_harvest_fill_tube_heater')
    }))
  end)
end)
