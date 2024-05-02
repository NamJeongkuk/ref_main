return personality({
  refrigerator_model = import("../RefrigeratorModel/refrigerator_model.lua"),
  platform = import("../Platform/side_by_side_platform.lua"),
  defrost = import("../Defrost/single_evaporator_defrost.lua"),
  enhanced_sabbath = import("../EnhancedSabbath/enhanced_sabbath.lua"),
  fans = import("../Fan/fan_list.lua"),
  fresh_food_and_freezer_grid = import("../Grid/single_evap_grid.lua"),
  feature_pan_grid = import("../Grid/feature_pan_grid.lua"),
  ice_cabinet_grid = import("../Grid/ice_cabinet_grid.lua"),
  sabbath = import("../Sabbath/sabbath.lua"),
  convertible_compartment = import("../ConvertibleCompartment/convertible_compartment.lua"),
  feature_pan = import("../FeaturePan/feature_pan.lua"),
  ice_cabinet = import("../IceCabinet/ice_cabinet.lua"),
  sensors = import("../Sensor/sensors.lua"),
  system_monitor = import("../SystemMonitor/system_monitor.lua"),
  compressor = import("../Compressor/variable_speed_cooling_mode_dependent_compressor.lua"),
  setpoint = import("../Setpoints/setpoint.lua"),
  fresh_food_thermal_offset = import('../Setpoints/ThermalOffset/FreshFood/fresh_food_thermal_offset.lua'),
  freezer_thermal_offset = import('../Setpoints/ThermalOffset/Freezer/freezer_thermal_offset.lua'),
  feature_pan_thermal_offset = import('../Setpoints/ThermalOffset/FeaturePan/convertible_compartment_thermal_offset.lua'),
  ice_cabinet_thermal_offset = import('../Setpoints/ThermalOffset/IceCabinet/ice_cabinet_thermal_offset.lua'),
  shift_offset_calculator = import('../Setpoints/ThermalOffset/shift_offset_calculator.lua'),
  bsp_configuration = import("../Bsp/bsp_configuration_sxs.lua"),
  damper = import("../Damper/fresh_food_damper.lua"),
  damper_heater = import("../Damper/damper_heater_with_false_flag_for_following_defrost.lua"),
  pulldown = import("../Grid/pulldown.lua"),
  ice_maker = import("../IceMaker/ice_maker_sxs.lua"),
  flow_meter = import("../FlowMeter/flow_meter.lua"),
  dispenser = import("../Dispenser/dispenser.lua"),
  recess_heater = import("../RecessHeater/recess_heater.lua"),
  turbo_mode_setpoint = import("../TurboModes/turbo_mode_setpoint.lua"),
  lighting = import("../Lighting/lighting.lua"),
  load_off_door_open_compartment_list = import("../Cooling/load_off_door_open_compartment_list.lua"),
  filter_data = import("../FilterData/filter_data.lua"),
  water_valve_flow_rate = import("../WaterValveFlowRate/water_valve_flow_rate.lua"),
  cabinet_temperature_exceeded = import("../Cooling/cabinet_temperature_exceeded.lua"),
  sealed_system_valve = import("../SealedSystemValve/sealed_system_valve.lua"),
  fresh_food_non_heated_cycle_defrost = import("../NonHeatedCycleDefrost/fresh_food_non_heated_cycle_defrost.lua")
})
