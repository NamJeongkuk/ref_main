return personality({
  defrost = import("../Defrost/single_evaporator_defrost.lua"),
  enhancedSabbath = import("../EnhancedSabbath/enhanced_sabbath.lua"),
  fans = import("../Fan/fan_list.lua"),
  grid = import("../Grid/single_evap_grid_integration.lua"),
  sabbath = import("../Sabbath/sabbath.lua"),
  convertibleCompartment = import("../ConvertibleCompartment/convertible_compartment.lua"),
  evaporator = import("../Evaporator/single_evaporator.lua"),
  sensors = import("../Sensor/sensors.lua"),
  systemMonitor = import("../SystemMonitor/system_monitor.lua"),
  compressor = import("../Compressor/single_speed_compressor.lua"),
  setpoint = import("../Setpoints/setpoint.lua"),
  cabinet_offset = import("../Setpoints/AdjustedSetpoint/CabinetOffset/cabinet_offset.lua"),
  bsp_configuration = import("../Bsp/bsp_configuration.lua"),
  single_damper = import("../Damper/fresh_food_damper.lua"),
  damper_heater = import("../Damper/damper_heater.lua"),
  pulldown = import("../Grid/pulldown.lua"),
  ice_maker = import("../IceMaker/ice_maker.lua"),
  flow_meter = import("../FlowMeter/flow_meter.lua"),
  dispenser = import("../Dispenser/dispenser.lua"),
  recess_heater = import("../RecessHeater/recess_heater.lua"),
  turbo_mode_setpoint = import("../TurboModes/turbo_mode_setpoint.lua"),
  load_off_door_open_compartment_list = import("../Cooling/load_off_door_open_compartment_list.lua")
})
