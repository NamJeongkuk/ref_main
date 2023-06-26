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
        pointer(config.defrost),
        pointer(config.enhancedSabbath),
        pointer(config.fans),
        pointer(config.grid),
        pointer(config.sabbath),
        pointer(config.convertibleCompartment),
        pointer(config.evaporator),
        pointer(config.sensors),
        pointer(config.systemMonitor),
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
        pointer(config.turbo_mode_setpoint)
      ))
  end)

  return function(config)
    validate_arguments(config, {
      defrost = { constraint.typed_string('defrost') },
      enhancedSabbath = { constraint.typed_string('enhancedSabbath') },
      fans = { constraint.typed_string('fan_list') },
      grid = { constraint.typed_string('grid') },
      sabbath = { constraint.typed_string('sabbath') },
      convertibleCompartment = { constraint.typed_string('convertibleCompartment') },
      evaporator = { constraint.typed_string('evaporator') },
      sensors = { constraint.typed_string('sensors') },
      systemMonitor = { constraint.typed_string('systemMonitor') },
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
      turbo_mode_setpoint = { constraint.typed_string('turbo_mode_setpoint') }
    })
    return generate(config)
  end
end
