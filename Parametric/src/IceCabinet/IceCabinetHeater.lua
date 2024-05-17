local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local ice_cabinet_heater_id = require 'IceCabinet/IceCabinetHeaterId'

local ice_cabinet_heaters = {
  [ice_cabinet_heater_id.duct_door_heater] = { 'duct_door_heater' },
  [ice_cabinet_heater_id.ice_cabinet_gasket_heater] = { 'ice_cabinet_gasket_heater' },
  [ice_cabinet_heater_id.ice_duct_port_heater] = { 'ice_duct_port_heater' }
}

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      ice_cabinet_heaters[config.ice_cabinet_heater_id],
      structure(
        u8(config.ice_cabinet_heater_min_duty_cycle_percentage),
        i16(config.ice_cabinet_heater_intercept_coefficient),
        i16(config.ice_cabinet_heater_ice_formation_mode_coefficient),
        i16(config.ice_cabinet_heater_ice_cabinet_fan_coefficient),
        i16(config.ice_cabinet_heater_freezer_setpoint_slope_coefficient)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        ice_cabinet_heater_id = { constraint.in_enumeration(ice_cabinet_heater_id) },
        ice_cabinet_heater_min_duty_cycle_percentage = { constraint.in_range(0,100) },
        ice_cabinet_heater_intercept_coefficient = { constraint.i16 },
        ice_cabinet_heater_ice_formation_mode_coefficient = { constraint.i16 },
        ice_cabinet_heater_ice_cabinet_fan_coefficient = { constraint.i16 },
        ice_cabinet_heater_freezer_setpoint_slope_coefficient = { constraint.i16 }
      }
    )
    return generate(config)
  end
end
