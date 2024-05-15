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
        pointer(config.platform),
        pointer(config.defrost),
        pointer(config.enhanced_sabbath),
        pointer(config.fans),
        pointer(config.fresh_food_and_freezer_grid),
        pointer(config.fresh_food_and_freezer_grid_deltas),
        i16(config.fresh_food_minimum_cross_ambient_adjusted_hysteresis_in_degfx100),
        u16(config.fresh_food_cross_ambient_hysteresis_coefficient_in_degfx1000_over_degf),
        pointer(config.feature_pan_grid),
        pointer(config.feature_pan_grid_deltas),
        pointer(config.ice_cabinet_grid),
        pointer(config.ice_cabinet_grid_deltas),
        pointer(config.sabbath),
        pointer(config.convertible_compartment),
        pointer(config.feature_pan),
        pointer(config.ice_cabinet),
        pointer(config.sensors),
        pointer(config.system_monitor),
        pointer(config.compressor),
        pointer(config.setpoint),
        pointer(config.fresh_food_thermal_offset),
        pointer(config.freezer_thermal_offset),
        pointer(config.feature_pan_thermal_offset),
        pointer(config.ice_cabinet_thermal_offset),
        pointer(config.shift_offset_calculator),
        pointer(config.bsp_configuration),
        pointer(config.damper),
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
        pointer(config.sealed_system_valve),
        pointer(config.fresh_food_non_heated_cycle_defrost)
      ))
  end)

  return function(config)
    validate_arguments(config, {
      refrigerator_model = { constraint.typed_string('refrigerator_model') },
      platform = { constraint.typed_string('platform') },
      defrost = { constraint.typed_string('defrost') },
      enhanced_sabbath = { constraint.typed_string('enhanced_sabbath') },
      fans = { constraint.typed_string('fan_list') },
      fresh_food_and_freezer_grid = { constraint.typed_string('fresh_food_and_freezer_grid') },
      fresh_food_and_freezer_grid_deltas = { constraint.typed_string('grid_deltas') },
      fresh_food_minimum_cross_ambient_adjusted_hysteresis_in_degfx100 = { constraint.i16 },
      fresh_food_cross_ambient_hysteresis_coefficient_in_degfx1000_over_degf = { constraint.u16 },
      feature_pan_grid = { constraint.typed_string('feature_pan_grid') },
      feature_pan_grid_deltas = { constraint.typed_string('grid_deltas') },
      ice_cabinet_grid = { constraint.typed_string('ice_cabinet_grid') },
      ice_cabinet_grid_deltas = { constraint.typed_string('grid_deltas') },
      sabbath = { constraint.typed_string('sabbath') },
      convertible_compartment = { constraint.typed_string('convertible_compartment') },
      feature_pan = { constraint.typed_string('feature_pan') },
      ice_cabinet = { constraint.typed_string('ice_cabinet') },
      sensors = { constraint.typed_string('sensors') },
      system_monitor = { constraint.typed_string('system_monitor') },
      compressor = { constraint.typed_string('compressor') },
      setpoint = { constraint.typed_string('setpoint') },
      fresh_food_thermal_offset = { constraint.typed_string('fresh_food_thermal_offset') },
      freezer_thermal_offset = { constraint.typed_string('freezer_thermal_offset') },
      feature_pan_thermal_offset = { constraint.typed_string('feature_pan_thermal_offset') },
      ice_cabinet_thermal_offset = { constraint.typed_string('ice_cabinet_thermal_offset') },
      shift_offset_calculator = { constraint.typed_string('shift_offset_calculator') },
      bsp_configuration = { constraint.typed_string('bsp_configuration') },
      damper = { constraint.typed_string('damper') },
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
      sealed_system_valve = { constraint.typed_string('sealed_system_valve') },
      fresh_food_non_heated_cycle_defrost = { constraint.typed_string('fresh_food_non_heated_cycle_defrost') }
    })
    return generate(config)
  end
end
