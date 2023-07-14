local IceMaker = require 'IceMaker/IceMaker'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('ice_maker', function()
  local ice_maker = IceMaker(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      freezer_ice_rate = TypedString('freezer_ice_rate', 'freezer_ice_rate'),
      aluminum_mold_ice_maker = TypedString('aluminum_mold_ice_maker', 'aluminum_mold_ice_maker'),
      twist_tray_ice_maker = TypedString('twist_tray_ice_maker', 'twist_tray_ice_maker'),
      auger_motor = TypedString('auger_motor', 'auger_motor'),
      non_harvest_fill_tube_heater = TypedString('non_harvest_fill_tube_heater', 'non_harvest_fill_tube_heater')
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(ice_maker, generate_config())
  end)

  it('should assert if freezer_ice_rate is not a string', function()
    should_fail_with('freezer_ice_rate must be a typed string with type freezer_ice_rate, but is a number', function()
      ice_maker(generate_config({
        freezer_ice_rate = 1
      }))
    end)
  end)

  it('should assert if aluminum_mold_ice_maker is not a string', function()
    should_fail_with('aluminum_mold_ice_maker must be a typed string with type aluminum_mold_ice_maker, but is a number', function()
      ice_maker(generate_config({
        aluminum_mold_ice_maker = 2
      }))
    end)
  end)

  it('should assert if twist_tray_ice_maker is not a string', function()
    should_fail_with('twist_tray_ice_maker must be a typed string with type twist_tray_ice_maker, but is a number', function()
      ice_maker(generate_config({
        twist_tray_ice_maker = 3
      }))
    end)
  end)

  it('should assert if auger_motor is not a string', function()
    should_fail_with('auger_motor must be a typed string with type auger_motor, but is a number', function()
      ice_maker(generate_config({
        auger_motor = 4
      }))
    end)
  end)

  it('should assert if non_harvest_fill_tube_heater is not a string', function()
    should_fail_with('non_harvest_fill_tube_heater must be a typed string with type non_harvest_fill_tube_heater, but is a number', function()
      ice_maker(generate_config({
        non_harvest_fill_tube_heater = 5
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for freezer ice rate', function()
    local expected = remove_whitespace([[
        structure(
          pointer(freezer_ice_rate),
          pointer(aluminum_mold_ice_maker),
          pointer(twist_tray_ice_maker),
          pointer(auger_motor),
          pointer(non_harvest_fill_tube_heater)
        )
      ]])

    local actual = ice_maker({
      freezer_ice_rate = TypedString('freezer_ice_rate', 'freezer_ice_rate'),
      aluminum_mold_ice_maker = TypedString('aluminum_mold_ice_maker', 'aluminum_mold_ice_maker'),
      twist_tray_ice_maker = TypedString('twist_tray_ice_maker', 'twist_tray_ice_maker'),
      auger_motor = TypedString('auger_motor', 'auger_motor'),
      non_harvest_fill_tube_heater = TypedString('non_harvest_fill_tube_heater', 'non_harvest_fill_tube_heater')
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('ice_maker'))
  end)

  it('should memoize', function()
    should_memoize_calls(ice_maker, generate_config())
  end)
end)
