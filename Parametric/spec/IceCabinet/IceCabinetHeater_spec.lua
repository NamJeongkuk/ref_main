local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local IceCabinetHeater = require 'IceCabinet/IceCabinetHeater'
local ice_cabinet_heater_id = require 'IceCabinet/IceCabinetHeaterId'

describe('IceCabinetHeater', function()
  local ice_cabinet_heater = IceCabinetHeater(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      ice_cabinet_heater_id = ice_cabinet_heater_id.duct_door_heater,
      ice_cabinet_heater_min_duty_cycle_percentage = 10,
      ice_cabinet_heater_intercept_coefficient = 1,
      ice_cabinet_heater_ice_formation_mode_coefficient = 2,
      ice_cabinet_heater_ice_cabinet_fan_coefficient = 3,
      ice_cabinet_heater_freezer_setpoint_slope_coefficient = 4
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(ice_cabinet_heater, generate_config())
  end)

  it('should assert if ice_cabinet_heater_id is not an ice_cabinet_heater_id', function()
    should_fail_with('ice_cabinet_heater_id=totally_not_an_ice_cabinet_heater_id is not in the ice_cabinet_heater_id enumeration, allowable values: duct_door_heater, ice_cabinet_gasket_heater, ice_duct_port_heater', function()
      ice_cabinet_heater(generate_config({
        ice_cabinet_heater_id = 'totally_not_an_ice_cabinet_heater_id'
      }))
    end)
  end)

  it('should assert if ice_cabinet_heater_min_duty_cycle_percentage is not in range', function()
    should_fail_with('ice_cabinet_heater_min_duty_cycle_percentage=101 must be in [0, 100]', function()
      ice_cabinet_heater(generate_config({
        ice_cabinet_heater_min_duty_cycle_percentage = 101
      }))
    end)
  end)

  it('should assert if ice_cabinet_heater_intercept_coefficient is not in range', function()
    should_fail_with('ice_cabinet_heater_intercept_coefficient=32768 must be in [-32768, 32767]', function()
      ice_cabinet_heater(generate_config({
        ice_cabinet_heater_intercept_coefficient = 32768
      }))
    end)
  end)

  it('should assert if ice_cabinet_heater_ice_formation_mode_coefficient is not in range', function()
    should_fail_with('ice_cabinet_heater_ice_formation_mode_coefficient=32768 must be in [-32768, 32767]', function()
      ice_cabinet_heater(generate_config({
        ice_cabinet_heater_ice_formation_mode_coefficient = 32768
      }))
    end)
  end)

  it('should assert if ice_cabinet_heater_ice_cabinet_fan_coefficient is not in range', function()
    should_fail_with('ice_cabinet_heater_ice_cabinet_fan_coefficient=32768 must be in [-32768, 32767]', function()
      ice_cabinet_heater(generate_config({
        ice_cabinet_heater_ice_cabinet_fan_coefficient = 32768
      }))
    end)
  end)

  it('should assert if ice_cabinet_heater_freezer_setpoint_slope_coefficient is not in range', function()
    should_fail_with('ice_cabinet_heater_freezer_setpoint_slope_coefficient=32768 must be in [-32768, 32767]', function()
      ice_cabinet_heater(generate_config({
        ice_cabinet_heater_freezer_setpoint_slope_coefficient = 32768
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for duct_door_heater', function()
    local expected = remove_whitespace([[
      structure(
        u8(10),
        i16(1),
        i16(2),
        i16(3),
        i16(4)
      )
    ]])

    local actual = ice_cabinet_heater({
      ice_cabinet_heater_id = ice_cabinet_heater_id.duct_door_heater,
      ice_cabinet_heater_min_duty_cycle_percentage = 10,
      ice_cabinet_heater_intercept_coefficient = 1,
      ice_cabinet_heater_ice_formation_mode_coefficient = 2,
      ice_cabinet_heater_ice_cabinet_fan_coefficient = 3,
      ice_cabinet_heater_freezer_setpoint_slope_coefficient = 4
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('duct_door_heater'))
  end)

  it('should generate a typed string with the correct data and type for ice_cabinet_gasket_heater', function()
    local expected = remove_whitespace([[
      structure(
        u8(10),
        i16(1),
        i16(2),
        i16(3),
        i16(4)
      )
    ]])

    local actual = ice_cabinet_heater({
      ice_cabinet_heater_id = ice_cabinet_heater_id.ice_cabinet_gasket_heater,
      ice_cabinet_heater_min_duty_cycle_percentage = 10,
      ice_cabinet_heater_intercept_coefficient = 1,
      ice_cabinet_heater_ice_formation_mode_coefficient = 2,
      ice_cabinet_heater_ice_cabinet_fan_coefficient = 3,
      ice_cabinet_heater_freezer_setpoint_slope_coefficient = 4
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('ice_cabinet_gasket_heater'))
  end)

  it('should generate a typed string with the correct data and type for ice_duct_port_heater', function()
    local expected = remove_whitespace([[
      structure(
        u8(10),
        i16(1),
        i16(2),
        i16(3),
        i16(4)
      )
    ]])

    local actual = ice_cabinet_heater({
      ice_cabinet_heater_id = ice_cabinet_heater_id.ice_duct_port_heater,
      ice_cabinet_heater_min_duty_cycle_percentage = 10,
      ice_cabinet_heater_intercept_coefficient = 1,
      ice_cabinet_heater_ice_formation_mode_coefficient = 2,
      ice_cabinet_heater_ice_cabinet_fan_coefficient = 3,
      ice_cabinet_heater_freezer_setpoint_slope_coefficient = 4
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('ice_duct_port_heater'))
  end)

  it('should memoize', function()
    should_memoize_calls(ice_cabinet_heater, generate_config())
  end)
end)
