local IceCabinet = require 'IceCabinet/IceCabinet'
local GammaTable = require 'IceCabinet/GammaTable'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('IceCabinet', function()
  local ice_cabinet = IceCabinet(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      gamma_table = TypedString('gamma_table', 'gamma_table'),
      ice_cabinet_fan_startup = TypedString('ice_cabinet_fan_startup', 'ice_cabinet_fan_startup'),
      freezer_setpoint_to_ice_cabinet_setpoint_table = TypedString('freezer_setpoint_to_ice_cabinet_setpoint_table', 'freezer_setpoint_to_ice_cabinet_setpoint_table')
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(ice_cabinet, generate_config())
  end)

  it('should assert if gamma_table is not a gamma_table', function()
    should_fail_with('gamma_table must be a typed string with type gamma_table, but is a number', function()
      ice_cabinet(generate_config({
        gamma_table = -1
      }))
    end)
  end)

  it('should assert if ice_cabinet_fan_startup is not a ice_cabinet_fan_startup', function()
    should_fail_with('ice_cabinet_fan_startup must be a typed string with type ice_cabinet_fan_startup, but is a number', function()
      ice_cabinet(generate_config({
        ice_cabinet_fan_startup = -1
      }))
    end)
  end)

  it('should assert if freezer_setpoint_to_ice_cabinet_setpoint_table is not a freezer_setpoint_to_ice_cabinet_setpoint_table', function()
    should_fail_with('freezer_setpoint_to_ice_cabinet_setpoint_table must be a typed string with type freezer_setpoint_to_ice_cabinet_setpoint_table, but is a number', function()
      ice_cabinet(generate_config({
        freezer_setpoint_to_ice_cabinet_setpoint_table = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type ice_cabinet', function()
    local expected = remove_whitespace([[
      structure(
        pointer(gamma_table),
        pointer(ice_cabinet_fan_startup),
        pointer(freezer_setpoint_to_ice_cabinet_setpoint_table)
      )
    ]])

    local actual = ice_cabinet(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('ice_cabinet'))
  end)

  it('should memoize', function()
    should_memoize_calls(ice_cabinet, generate_config())
  end)
end)
