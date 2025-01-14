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
        pointer(config.convertible_compartment_as_freezer),
        pointer(config.convertible_compartment_as_fresh_food),
        pointer(config.freezer_evap_thermistor),
        pointer(config.fresh_food_evap_thermistor),
        pointer(config.ambient_thermistor),
        pointer(config.ambient_humidity_sensor),
        pointer(config.convertible_compartment_evap_thermistor),
        pointer(config.ice_maker_0_mold_thermistor),
        pointer(config.ice_maker_1_mold_thermistor),
        pointer(config.ice_maker_2_mold_thermistor),
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
        convertible_compartment_as_freezer = { constraint.typed_string('sensor_type') },
        convertible_compartment_as_fresh_food = { constraint.typed_string('sensor_type') },
        freezer_evap_thermistor = { constraint.typed_string('sensor_type') },
        fresh_food_evap_thermistor = { constraint.typed_string('sensor_type') },
        ambient_thermistor = { constraint.typed_string('sensor_type') },
        ambient_humidity_sensor = { constraint.typed_string('humidity_sensor_type') },
        convertible_compartment_evap_thermistor = { constraint.typed_string('sensor_type') },
        ice_maker_0_mold_thermistor = { constraint.typed_string('sensor_type') },
        ice_maker_1_mold_thermistor = { constraint.typed_string('sensor_type') },
        ice_maker_2_mold_thermistor = { constraint.typed_string('sensor_type') },
        periodic_update_rate_in_msec = { constraint.u16 }
      })

    return generate(config)
  end
end
