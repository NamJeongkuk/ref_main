return personality({
  refrigerator_model = import("../RefrigeratorModel/refrigerator_model.lua"),
  defrost = import("../Defrost/single_evaporator_defrost_qa.lua"),
  enhanced_sabbath = import("../EnhancedSabbath/enhanced_sabbath_number_of_fresh_food_defrosts_zero_qa.lua"),
  fans = import("../Fan/fan_list_qa.lua"),
  grid = import("../Grid/single_evap_grid_qa.lua"),
  sabbath = import("../Sabbath/sabbath_qa.lua"),
  convertible_compartment = import("../ConvertibleCompartment/convertible_compartment.lua"),
  evaporator = import("../Evaporator/single_evaporator.lua"),
  sensors = import("../Sensor/sensors_qa.lua"),
  system_monitor = import("../SystemMonitor/system_monitor_qa.lua"),
  compressor = import("../Compressor/single_speed_compressor_qa.lua"),
  setpoint = import("../Setpoints/setpoint.lua"),
  cabinet_offset = import("../Setpoints/AdjustedSetpoint/CabinetOffset/cabinet_offset.lua"),
  bsp_configuration = import("../Bsp/bsp_configuration.lua"),
  single_damper = import("../Damper/fresh_food_damper_qa.lua"),
  damper_heater = import("../Damper/damper_heater.lua"),
  pulldown = import("../Grid/pulldown.lua"),
  ice_maker = import("../IceMaker/ice_maker_qa.lua"),
  flow_meter = import("../FlowMeter/flow_meter.lua"),
  dispenser = import("../Dispenser/dispenser_qa.lua"),
  recess_heater = import("../RecessHeater/recess_heater.lua"),
  turbo_mode_setpoint = import("../TurboModes/turbo_mode_setpoint_qa.lua"),
  lighting = import("../Lighting/lighting.lua"),
  load_off_door_open_compartment_list = import("../Cooling/load_off_door_open_compartment_list.lua"),
  filter_data = import("../FilterData/filter_data.lua"),
  water_valve_flow_rate = import("../WaterValveFlowRate/water_valve_flow_rate.lua"),
  cabinet_temperature_exceeded = import("../Cooling/cabinet_temperature_exceeded.lua")
})
