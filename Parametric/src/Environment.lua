return function(directory)
  local import = require 'lua-common'.util.import

  import('ParametricData')
  import('Compressor/Compressor')
  import('Compressor/CompressorSpeeds')
  import('Compressor/CompressorTimes')
  import('Compressor/CoolingModeDependentCompressorSpeeds')
  import('Compressor/CoolingModeIndependentCompressorSpeeds')
  import('ConvertibleCompartment/ConvertibleCompartment')
  import('FeaturePan/FeaturePan')
  import('Defrost/Defrost')
  import('Dispenser/Dispenser')
  import('Dispenser/DoorInhibitDispenseTable')
  import('Dispenser/AugerMotor')
  import('EnhancedSabbath/EnhancedSabbath')
  import('Grid/Grid')
  import('Grid/Gridline')
  import('Grid/GridDeltas')
  import('Grid/GridIdType')
  import('Fan/CondenserFan')
  import('Fan/DutyCycle')
  import('Fan/FanList')
  import('Fan/Fan')
  import('Fan/FanId')
  import('Fan/FanFault')
  import('Fan/FanCareAboutCoolingModeSpeedTable')
  import('Fan/FanSpeedTable')
  import('Fan/Pid')
  import('Fan/Rpm')
  import('Personality/Personality')
  import('SealedSystemValve/SealedSystemValvePositionType')
  import('Grid/GridLineCorrection')
  import('Sabbath/Sabbath')
  import('Platform/Platform')
  import('Sensor/Sensors')
  import('Sensor/SensorType')
  import('Sensor/HumiditySensorType')
  import('SystemMonitor/SystemMonitor')
  import('Damper/DamperPositionType')
  import('Setpoints/Setpoint')
  import('Bsp/ErdMapping')
  import('Bsp/BspErd')
  import('Bsp/BspMappedErd')
  import('Bsp/BspConfiguration')
  import('Bsp/BinarySearchConfiguration')
  import('Bsp/LinearSearchConfiguration')
  import('Bsp/ErdListToSortedErdMappings')
  import('Setpoints/SetpointZone')
  import('Setpoints/AdjustedSetpoint')
  import('Setpoints/FreshFoodAdjustedSetpoint')
  import('Setpoints/FreezerAdjustedSetpoint')
  import('Setpoints/ConvertibleCompartmentAdjustedSetpoint')
  import('Setpoints/DeliPanAdjustedSetpoint')
  import('Setpoints/IceCabinetAdjustedSetpoint')
  import('Setpoints/CrossAmbientOffsetCalculator')
  import('Setpoints/ShiftOffsetCalculator')
  import('Setpoints/FixedSetpointOffset')
  import('Setpoints/SetpointOffset')
  import('Setpoints/ShiftOffset')
  import('Setpoints/CrossAmbientOffset')
  import('Setpoints/HighAmbientOffset')
  import('Setpoints/CabinetOffset')
  import('Damper/Damper')
  import('Damper/DamperId')
  import('Damper/DamperSteps')
  import('Damper/ConvertibleDamperStepData')
  import('Setpoints/UserSetpoint')
  import('Setpoints/UserSetpointData')
  import('Damper/DamperHeater')
  import('Grid/Pulldown')
  import('Grid/CrossAmbientHysteresisAdjustment')
  import('IceMaker/AluminumMoldIceMaker')
  import('IceMaker/IceMakerFillBlocker')
  import('IceMaker/FreezerIceRate')
  import('IceMaker/IceMakerFillMonitor')
  import('IceMaker/IceMaker')
  import('IceMaker/NonHarvestFillTubeHeater')
  import('IceMaker/IceMakerSlots')
  import('IceMaker/IceMakerSlot')
  import('IceMaker/IceMakerType')
  import('IceMaker/IceMakerLocation')
  import('FilterData/CommonFilterData')
  import('FilterData/FilterData')
  import('FilterData/RfidFilterData')
  import('FilterData/RfidFilterUpdateRate')
  import('FlowMeter/FlowMeter')
  import('IceMaker/TwistTrayIceMaker')
  import('IceMaker/HarvestCountCalculator')
  import('VariableSweatHeater/VariableSweatHeater')
  import('VariableSweatHeater/VariableSweatHeaterType1')
  import('VariableSweatHeater/VariableSweatHeaterType2')
  import('VariableSweatHeater/VariableSweatHeaterType3')
  import('VariableSweatHeater/VariableSweatHeaterType4')
  import('VariableSweatHeater/VariableSweatHeaterType5')
  import('VariableSweatHeater/VariableSweatHeaterType6')
  import('VariableSweatHeater/VariableSweatHeaterType7')
  import('WaterValveFlowRate/WaterValveFlowRate')
  import('RecessHeater/RecessHeater')
  import('TurboModes/FreshFoodTurboCoolSetpoint')
  import('TurboModes/FreezerTurboFreezeSetpoint')
  import('TurboModes/TurboModeSetpoint')
  import('RefrigeratorModel/RefrigeratorModel')
  import('Lighting/DoorLighting')
  import('Lighting/Lighting')
  import('Lighting/RampingPwmDutyCyclePercentageBundle')
  import('Lighting/UserAllowableInteriorLightingBitmap')
  import('Lighting/UserAllowableInteriorLighting')
  import('Cooling/LoadOffDoorOpen')
  import('Cooling/LoadOffDoorOpenCompartmentList')
  import('Cooling/CabinetTemperatureExceeded')
  import('SealedSystemValve/SealedSystemValve')
  import('NonHeatedCycleDefrost/FreshFoodNonHeatedCycleDefrost')
  import('constants')

  Core = require 'lua-parametric-tools'.common.Core
  enum = require 'lua-common'.utilities.enum
  math = require 'lua-common'.math

  core = Core()
  parametric_data = ParametricData(core)
  compressor = Compressor(core)
  compressorSpeeds = CompressorSpeeds(core)
  coolingModeIndependentCompressorSpeeds = CoolingModeIndependentCompressorSpeeds(core)
  coolingModeDependentCompressorSpeeds = CoolingModeDependentCompressorSpeeds(core)
  compressorTimes = CompressorTimes(core)
  convertible_compartment = ConvertibleCompartment(core)
  feature_pan = FeaturePan(core)
  cross_ambient_hysteresis_adjustment = CrossAmbientHysteresisAdjustment(core)
  defrost = Defrost(core)
  duty_cycle = DutyCycle(core)
  door_inhibit_dispense_table = DoorInhibitDispenseTable(core)
  enhanced_sabbath = EnhancedSabbath(core)
  fan = Fan(core)
  fan_fault = FanFault(core)
  fan_id = FanId
  fan_list = FanList(core)
  grid = Grid(core)
  gridline = Gridline(core)
  grid_deltas = GridDeltas(core)
  personality = Personality(core)
  pid = Pid(core)
  rpm = Rpm(core)
  sabbath = Sabbath(core)
  platform = Platform(core)
  sensors = Sensors(core)
  sensor_type = SensorType(core)
  humidity_sensor_type = HumiditySensorType(core)
  system_monitor = SystemMonitor(core)
  fan_care_about_cooling_mode_speed_table = FanCareAboutCoolingModeSpeedTable(core)
  fan_speed_table = FanSpeedTable(core)
  setpoint = Setpoint(core)
  erd_mapping = ErdMapping(core)
  bsp_configuration = BspConfiguration(core)
  binary_search_configuration = BinarySearchConfiguration(core)
  linear_search_configuration = LinearSearchConfiguration(core)
  setpoint_zone = SetpointZone(core)
  adjusted_setpoint = AdjustedSetpoint(core)
  fresh_food_adjusted_setpoint = FreshFoodAdjustedSetpoint(core)
  freezer_adjusted_setpoint = FreezerAdjustedSetpoint(core)
  convertible_compartment_adjusted_setpoint = ConvertibleCompartmentAdjustedSetpoint(core)
  deli_pan_adjusted_setpoint = DeliPanAdjustedSetpoint(core)
  ice_cabinet_adjusted_setpoint = IceCabinetAdjustedSetpoint(core)
  cross_ambient_offset_calculator = CrossAmbientOffsetCalculator(core)
  shift_offset_calculator = ShiftOffsetCalculator(core)
  fixed_setpoint_offset = FixedSetpointOffset(core)
  setpoint_offset = SetpointOffset(core)
  shift_offset = ShiftOffset(core)
  cross_ambient_offset = CrossAmbientOffset(core)
  high_ambient_offset = HighAmbientOffset(core)
  cabinet_offset = CabinetOffset(core)
  damper = Damper(core)
  damper_id = DamperId
  damper_steps = DamperSteps(core)
  convertible_damper_step_data = ConvertibleDamperStepData(core)
  user_setpoint = UserSetpoint(core)
  user_setpoint_data = UserSetpointData(core)
  damper_heater = DamperHeater(core)
  pulldown = Pulldown(core)
  aluminum_mold_ice_maker = AluminumMoldIceMaker(core)
  freezer_ice_rate = FreezerIceRate(core)
  ice_maker_fill_blocker = IceMakerFillBlocker(core)
  ice_maker_fill_monitor = IceMakerFillMonitor(core)
  ice_maker_slots = IceMakerSlots(core)
  ice_maker_slot = IceMakerSlot(core)
  ice_maker_type = IceMakerType
  ice_maker_location = IceMakerLocation
  ice_maker = IceMaker(core)
  non_harvest_fill_tube_heater = NonHarvestFillTubeHeater(core)
  common_filter_data = CommonFilterData(core)
  filter_data = FilterData(core)
  rfid_filter_data = RfidFilterData(core)
  rfid_filter_update_rate = RfidFilterUpdateRate(core)
  flow_meter = FlowMeter(core)
  erd_list_to_sorted_erd_mappings = ErdListToSortedErdMappings(core)
  twist_tray_ice_maker = TwistTrayIceMaker(core)
  twist_tray_ice_maker_fill_monitor = IceMakerFillMonitor(core)
  dispenser = Dispenser(core)
  auger_motor = AugerMotor(core)
  variable_sweat_heater = VariableSweatHeater(core)
  variable_sweat_heater_type_1 = VariableSweatHeaterType1(core)
  variable_sweat_heater_type_2 = VariableSweatHeaterType2(core)
  variable_sweat_heater_type_3 = VariableSweatHeaterType3(core)
  variable_sweat_heater_type_4 = VariableSweatHeaterType4(core)
  variable_sweat_heater_type_5 = VariableSweatHeaterType5(core)
  variable_sweat_heater_type_6 = VariableSweatHeaterType6(core)
  variable_sweat_heater_type_7 = VariableSweatHeaterType7(core)
  water_valve_flow_rate = WaterValveFlowRate(core)
  recess_heater = RecessHeater(core)
  condenser_fan = CondenserFan(core)
  turbo_mode_setpoint = TurboModeSetpoint(core)
  fresh_food_turbo_cool_setpoint = FreshFoodTurboCoolSetpoint(core)
  freezer_turbo_freeze_setpoint = FreezerTurboFreezeSetpoint(core)
  harvest_count_calculator = HarvestCountCalculator(core)
  refrigerator_model = RefrigeratorModel(core)
  door_lighting = DoorLighting(core)
  lighting = Lighting(core)
  ramping_pwm_duty_cycle_percentage_bundle = RampingPwmDutyCyclePercentageBundle(core)
  user_allowable_interior_lighting_bitmap = UserAllowableInteriorLightingBitmap(core)
  user_allowable_interior_lighting = UserAllowableInteriorLighting(core)
  load_off_door_open = LoadOffDoorOpen(core)
  load_off_door_open_compartment_list = LoadOffDoorOpenCompartmentList(core)
  cabinet_temperature_exceeded = CabinetTemperatureExceeded(core)
  sealed_system_valve = SealedSystemValve(core)
  fresh_food_non_heated_cycle_defrost = FreshFoodNonHeatedCycleDefrost(core)

  return {
    core = core,
    parametric_data = parametric_data,
    compressor = compressor,
    compressor_speeds = compressorSpeeds,
    compressor_times = compressorTimes,
    cooling_mode_independent_compressor_speeds = coolingModeIndependentCompressorSpeeds,
    cooling_mode_dependent_compressor_speeds = coolingModeDependentCompressorSpeeds,
    convertible_compartment = convertible_compartment,
    feature_pan = feature_pan,
    cross_ambient_hysteresis_adjustment = cross_ambient_hysteresis_adjustment,
    defrost = defrost,
    duty_cycle = duty_cycle,
    door_inhibit_dispense_table = door_inhibit_dispense_table,
    enhanced_sabbath = enhanced_sabbath,
    fan = fan,
    fan_fault = fan_fault,
    fan_id = fan_id,
    fan_list = fan_list,
    grid = grid,
    grid_deltas = grid_deltas,
    gridline = gridline,
    grid_id_type = GridIdType,
    personality = personality,
    pid = pid,
    rpm = rpm,
    sealed_system_valve_position_type = SealedSystemValvePositionType,
    grid_line_correction = GridLineCorrection,
    sabbath = sabbath,
    platform = platform,
    sensors = sensors,
    sensor_type = sensor_type,
    humidity_sensor_type = humidity_sensor_type,
    system_monitor = system_monitor,
    fan_care_about_cooling_mode_speed_table = fan_care_about_cooling_mode_speed_table,
    fan_speed_table = fan_speed_table,
    damper_position_type = DamperPositionType,
    setpoint = setpoint,
    erd_mapping = erd_mapping,
    bsp_erd = BspErd,
    bsp_mapped_erd = BspMappedErd,
    bsp_configuration = bsp_configuration,
    binary_search_configuration = binary_search_configuration,
    linear_search_configuration = linear_search_configuration,
    setpoint_zone = setpoint_zone,
    adjusted_setpoint = adjusted_setpoint,
    fresh_food_adjusted_setpoint = fresh_food_adjusted_setpoint,
    freezer_adjusted_setpoint = freezer_adjusted_setpoint,
    convertible_compartment_adjusted_setpoint = convertible_compartment_adjusted_setpoint,
    deli_pan_adjusted_setpoint = deli_pan_adjusted_setpoint,
    ice_cabinet_adjusted_setpoint = ice_cabinet_adjusted_setpoint,
    cross_ambient_offset_calculator = cross_ambient_offset_calculator,
    shift_offset_calculator = shift_offset_calculator,
    fixed_setpoint_offset = fixed_setpoint_offset,
    setpoint_offset = setpoint_offset,
    shift_offset = shift_offset,
    cross_ambient_offset = cross_ambient_offset,
    high_ambient_offset = high_ambient_offset,
    cabinet_offset = cabinet_offset,
    damper = damper,
    damper_id = damper_id,
    damper_steps = damper_steps,
    convertible_damper_step_data = convertible_damper_step_data,
    user_setpoint = user_setpoint,
    user_setpoint_data = user_setpoint_data,
    damper_heater = damper_heater,
    pulldown = pulldown,
    aluminum_mold_ice_maker = aluminum_mold_ice_maker,
    freezer_ice_rate = freezer_ice_rate,
    ice_maker_fill_blocker = ice_maker_fill_blocker,
    ice_maker_fill_monitor = ice_maker_fill_monitor,
    ice_maker_slots = ice_maker_slots,
    ice_maker_slot = ice_maker_slot,
    ice_maker_type = ice_maker_type,
    ice_maker_location = ice_maker_location,
    ice_maker = ice_maker,
    non_harvest_fill_tube_heater = non_harvest_fill_tube_heater,
    common_filter_data = common_filter_data,
    filter_data = filter_data,
    rfid_filter_data = rfid_filter_data,
    rfid_filter_update_rate = rfid_filter_update_rate,
    flow_meter = flow_meter,
    twist_tray_ice_maker = twist_tray_ice_maker,
    twist_tray_ice_maker_fill_monitor = twist_tray_ice_maker_fill_monitor,
    dispenser = dispenser,
    auger_motor = auger_motor,
    variable_sweat_heater = variable_sweat_heater,
    variable_sweat_heater_type_1 = variable_sweat_heater_type_1,
    variable_sweat_heater_type_2 = variable_sweat_heater_type_2,
    variable_sweat_heater_type_3 = variable_sweat_heater_type_3,
    variable_sweat_heater_type_4 = variable_sweat_heater_type_4,
    variable_sweat_heater_type_5 = variable_sweat_heater_type_5,
    variable_sweat_heater_type_6 = variable_sweat_heater_type_6,
    variable_sweat_heater_type_7 = variable_sweat_heater_type_7,
    water_valve_flow_rate = water_valve_flow_rate,
    recess_heater = recess_heater,
    condenser_fan = condenser_fan,
    turbo_mode_setpoint = turbo_mode_setpoint,
    fresh_food_turbo_cool_setpoint = fresh_food_turbo_cool_setpoint,
    freezer_turbo_freeze_setpoint = freezer_turbo_freeze_setpoint,
    harvest_count_calculator = harvest_count_calculator,
    refrigerator_model = refrigerator_model,
    door_lighting = door_lighting,
    lighting = lighting,
    ramping_pwm_duty_cycle_percentage_bundle = ramping_pwm_duty_cycle_percentage_bundle,
    user_allowable_interior_lighting_bitmap = user_allowable_interior_lighting_bitmap,
    user_allowable_interior_lighting = user_allowable_interior_lighting,
    enum = enum,
    erd_list_to_sorted_erd_mappings = erd_list_to_sorted_erd_mappings,
    load_off_door_open = load_off_door_open,
    load_off_door_open_compartment_list = load_off_door_open_compartment_list,
    cabinet_temperature_exceeded = cabinet_temperature_exceeded,
    sealed_system_valve = sealed_system_valve,
    fresh_food_non_heated_cycle_defrost = fresh_food_non_heated_cycle_defrost,
    constants = constants,
    math = math,
    import = require 'lua-parametric-tools'.util.EvalInCurrentEnvironment(directory, {
      remove_tail_calls = true
    })
  }
end
