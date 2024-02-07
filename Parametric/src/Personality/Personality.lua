local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'parametric_personality' },
      structure(
        pointer(config.refrigerator_model),
        pointer(config.defrost),
        pointer(config.enhanced_sabbath),
        pointer(config.fans),
        pointer(config.grid),
        pointer(config.sabbath),
        pointer(config.convertible_compartment),
        pointer(config.evaporator),
        pointer(config.sensors),
        pointer(config.system_monitor),
        pointer(config.compressor),
        pointer(config.setpoint),
        pointer(config.cabinet_offset),
        pointer(config.bsp_configuration),
        pointer(config.single_damper),
        pointer(config.damper_heater),
        pointer(config.pulldown),
        pointer(config.ice_maker),
        pointer(config.flow_meter),
        pointer(config.dispenser),
        pointer(config.recess_heater),
        pointer(config.turbo_mode_setpoint),
        pointer(config.lighting),
        pointer(config.load_off_door_open_compartment_list),
        pointer(config.filter_data),
        pointer(config.water_valve_flow_rate),
        pointer(config.cabinet_temperature_exceeded),
        pointer(config.sealed_system_valve)
      ))
  end)

  return function(config)
    validate_arguments(config, {
      refrigerator_model = { constraint.typed_string('refrigerator_model') },
      defrost = { constraint.typed_string('defrost') },
      enhanced_sabbath = { constraint.typed_string('enhanced_sabbath') },
      fans = { constraint.typed_string('fan_list') },
      grid = { constraint.typed_string('grid') },
      sabbath = { constraint.typed_string('sabbath') },
      convertible_compartment = { constraint.typed_string('convertible_compartment') },
      evaporator = { constraint.typed_string('evaporator') },
      sensors = { constraint.typed_string('sensors') },
      system_monitor = { constraint.typed_string('system_monitor') },
      compressor = { constraint.typed_string('compressor') },
      setpoint = { constraint.typed_string('setpoint') },
      cabinet_offset = { constraint.typed_string('cabinet_offset') },
      bsp_configuration = { constraint.typed_string('bsp_configuration') },
      single_damper = { constraint.typed_string('single_damper') },
      damper_heater = { constraint.typed_string('damper_heater') },
      pulldown = { constraint.typed_string('pulldown') },
      ice_maker = { constraint.typed_string('ice_maker') },
      flow_meter = { constraint.typed_string('flow_meter') },
      dispenser = { constraint.typed_string('dispenser') },
      recess_heater = { constraint.typed_string('recess_heater') },
      turbo_mode_setpoint = { constraint.typed_string('turbo_mode_setpoint') },
      lighting = { constraint.typed_string('lighting') },
      load_off_door_open_compartment_list = { constraint.typed_string('load_off_door_open_compartment_list') },
      filter_data = { constraint.typed_string('filter_data') },
      water_valve_flow_rate = { constraint.typed_string('water_valve_flow_rate') },
      cabinet_temperature_exceeded = { constraint.typed_string('cabinet_temperature_exceeded') },
      sealed_system_valve = { constraint.typed_string('sealed_system_valve' ) }
    })
    return generate(config)
  end
end
