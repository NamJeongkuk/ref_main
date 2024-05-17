return personality({
  refrigerator_model = import("../RefrigeratorModel/refrigerator_model_convertible_compartment_double_drawer_bottom_freezer.lua"),
  platform = import("../Platform/four_door_dual_evap_platform.lua"),
  defrost = import("../Defrost/dual_evaporator_defrost_qa.lua"),
  enhanced_sabbath = import("../EnhancedSabbath/enhanced_sabbath_qa.lua"),
  fans = import("../Fan/fan_list_qa.lua"),
  fresh_food_and_freezer_grid = import("../Grid/dual_evap_grid_qa.lua"),
  fresh_food_and_freezer_grid_deltas = import("../Grid/grid_deltas_qa.lua"),
  fresh_food_minimum_cross_ambient_adjusted_hysteresis_in_degfx100 = 200,
  fresh_food_cross_ambient_hysteresis_coefficient_in_degfx1000_over_degf = 500,
  feature_pan_grid = import("../Grid/feature_pan_grid.lua"),
  feature_pan_grid_deltas = import("../Grid/grid_deltas_feature_pan.lua"),
  ice_cabinet_grid = import("../Grid/ice_cabinet_grid.lua"),
  ice_cabinet_grid_deltas = import("../Grid/grid_deltas_ice_cabinet.lua"),
  sabbath = import("../Sabbath/sabbath_qa.lua"),
  convertible_compartment = import("../ConvertibleCompartment/convertible_compartment.lua"),
  feature_pan = import("../FeaturePan/feature_pan.lua"),
  ice_cabinet = import("../IceCabinet/ice_cabinet.lua"),
  duct_door_heater = import("../IceCabinet/duct_door_heater.lua"),
  ice_cabinet_gasket_heater = import("../IceCabinet/ice_cabinet_gasket_heater.lua"),
  ice_duct_port_heater = import("../IceCabinet/ice_duct_port_heater.lua"),
  sensors = import("../Sensor/sensors_qa.lua"),
  system_monitor = import("../SystemMonitor/system_monitor_qa.lua"),
  compressor = import("../Compressor/variable_speed_cooling_mode_dependent_compressor_qa.lua"),
  setpoint = import("../Setpoints/setpoint.lua"),
  fresh_food_thermal_offset = import('../Setpoints/ThermalOffset/FreshFood/fresh_food_thermal_offset.lua'),
  freezer_thermal_offset = import('../Setpoints/ThermalOffset/Freezer/freezer_thermal_offset.lua'),
  feature_pan_thermal_offset = import('../Setpoints/ThermalOffset/FeaturePan/convertible_compartment_thermal_offset.lua'),
  ice_cabinet_thermal_offset = import('../Setpoints/ThermalOffset/IceCabinet/ice_cabinet_thermal_offset.lua'),
  shift_offset_calculator = import('../Setpoints/ThermalOffset/shift_offset_calculator.lua'),
  bsp_configuration = import("../Bsp/bsp_configuration_four_door_qa.lua"),
  damper = import("../Damper/convertible_damper_qa.lua"),
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
