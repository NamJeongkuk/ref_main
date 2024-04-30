return personality({
  refrigerator_model = import("../RefrigeratorModel/refrigerator_model_single_cabinet_fresh_food.lua"),
  platform = import("../Platform/all_fresh_food_platform.lua"),
  defrost = import("../Defrost/single_evaporator_defrost_qa.lua"),
  enhanced_sabbath = import("../EnhancedSabbath/enhanced_sabbath_qa.lua"),
  fans = import("../Fan/fan_list_qa.lua"),
  fresh_food_and_freezer_grid = import("../Grid/single_door_single_evap_qa.lua"),
  feature_pan_grid = import("../Grid/feature_pan_grid.lua"),
  ice_cabinet_grid = import("../Grid/ice_cabinet_grid.lua"),
  sabbath = import("../Sabbath/sabbath_qa.lua"),
  convertible_compartment = import("../ConvertibleCompartment/convertible_compartment.lua"),
  feature_pan = import("../FeaturePan/feature_pan.lua"),
  ice_cabinet = import("../IceCabinet/ice_cabinet.lua"),
  sensors = import("../Sensor/sensors_qa.lua"),
  system_monitor = import("../SystemMonitor/system_monitor_qa.lua"),
  compressor = import("../Compressor/single_speed_compressor_qa.lua"),
  setpoint = import("../Setpoints/setpoint.lua"),
  cabinet_offset = import("../Setpoints/AdjustedSetpoint/CabinetOffset/cabinet_offset.lua"),
  bsp_configuration = import("../Bsp/bsp_configuration_all_refrigerator_qa.lua"),
  damper = import("../Damper/fresh_food_damper_qa.lua"),
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
  cabinet_temperature_exceeded = import("../Cooling/cabinet_temperature_exceeded.lua"),
  sealed_system_valve = import("../SealedSystemValve/sealed_system_valve.lua"),
  fresh_food_non_heated_cycle_defrost = import("../NonHeatedCycleDefrost/fresh_food_non_heated_cycle_defrost.lua")
})
