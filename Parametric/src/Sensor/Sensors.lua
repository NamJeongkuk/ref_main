local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'sensors' },
      structure(
        pointer(config.freezer_cabinet_thermistor),
        pointer(config.fresh_food_cabinet_thermistor),
        pointer(config.freezer_evap_thermistor),
        pointer(config.fresh_food_evap_thermistor),
        pointer(config.convertible_compartment_cabinet_thermistor),
        pointer(config.ambient_thermistor),
        pointer(config.ambient_humidity_thermistor),
        pointer(config.convertible_compartment_evap_thermistor),
        pointer(config.aluminum_ice_maker_mold_thermistor),
        pointer(config.twist_tray_ice_maker_thermistor),
        u16(config.periodic_update_rate_in_msec)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        freezer_cabinet_thermistor = { constraint.typed_string('sensor_type') },
        fresh_food_cabinet_thermistor = { constraint.typed_string('sensor_type') },
        freezer_evap_thermistor = { constraint.typed_string('sensor_type') },
        fresh_food_evap_thermistor = { constraint.typed_string('sensor_type') },
        convertible_compartment_cabinet_thermistor = { constraint.typed_string('convertible_compartment_sensor_type') },
        ambient_thermistor = { constraint.typed_string('sensor_type') },
        ambient_humidity_thermistor = { constraint.typed_string('sensor_type') },
        convertible_compartment_evap_thermistor = { constraint.typed_string('sensor_type') },
        aluminum_ice_maker_mold_thermistor = { constraint.typed_string('sensor_type') },
        twist_tray_ice_maker_thermistor = { constraint.typed_string('sensor_type') },
        periodic_update_rate_in_msec = { constraint.u16 }
      })

    return generate(config)
  end
end
