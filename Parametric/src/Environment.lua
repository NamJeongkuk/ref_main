return function(directory)
  local import = require 'lua-common'.util.import

  import('ParametricData')
  import('Compressor/Compressor')
  import('Compressor/CompressorSpeeds')
  import('Compressor/CompressorTimes')
  import('Compressor/CoolingModeDependentCompressorSpeeds')
  import('Compressor/CoolingModeIndependentCompressorSpeeds')
  import('ConvertibleCompartment/ConvertibleCompartment')
  import('Defrost/Defrost')
  import('Dispenser/Dispenser')
  import('Dispenser/DoorInhibitDispenseTable')
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
  import('Evaporator/Evaporator')
  import('Sensor/Sensors')
  import('Sensor/SensorType')
  import('Sensor/ConvertibleCompartmentSensorType')
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
  import('Setpoints/IceBoxAdjustedSetpoint')
  import('Setpoints/CrossAmbientOffsetCalculator')
  import('Setpoints/ShiftOffsetCalculator')
  import('Setpoints/FixedSetpointOffset')
  import('Setpoints/SetpointOffset')
  import('Setpoints/ShiftOffset')
  import('Setpoints/CrossAmbientOffset')
  import('Damper/SingleDamper')
  import('Setpoints/UserSetpoint')
  import('Setpoints/UserSetpointData')
  import('Damper/DamperHeater')
  import('Grid/Pulldown')
  import('IceMaker/AluminumMoldIceMaker')
  import('IceMaker/FreezerIceRate')
  import('IceMaker/IceMakerFillMonitor')
  import('IceMaker/IceMaker')
  import('IceMaker/FillTubeHeater')
  import('IceMaker/AugerMotor')
  import('FlowMeter/FlowMeter')
  import('IceMaker/TwistTrayIceMaker')
  import('VariableSweatHeater/VariableSweatHeater')
  import('RecessHeater/RecessHeater')

  Core = require 'lua-parametric-tools'.common.Core
  enum = require 'lua-common'.utilities.enum

  core = Core()
  parametric_data = ParametricData(core)
  compressor = Compressor(core)
  compressorSpeeds = CompressorSpeeds(core)
  coolingModeIndependentCompressorSpeeds = CoolingModeIndependentCompressorSpeeds(core)
  coolingModeDependentCompressorSpeeds = CoolingModeDependentCompressorSpeeds(core)
  compressorTimes = CompressorTimes(core)
  convertibleCompartment = ConvertibleCompartment(core)
  defrost = Defrost(core)
  duty_cycle = DutyCycle(core)
  door_inhibit_dispense_table = DoorInhibitDispenseTable(core)
  enhancedSabbath = EnhancedSabbath(core)
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
  evaporator = Evaporator(core)
  sensors = Sensors(core)
  sensor_type = SensorType(core)
  convertible_compartment_sensor_type = ConvertibleCompartmentSensorType(core)
  systemMonitor = SystemMonitor(core)
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
  ice_box_adjusted_setpoint = IceBoxAdjustedSetpoint(core)
  cross_ambient_offset_calculator = CrossAmbientOffsetCalculator(core)
  shift_offset_calculator = ShiftOffsetCalculator(core)
  fixed_setpoint_offset = FixedSetpointOffset(core)
  setpoint_offset = SetpointOffset(core)
  shift_offset = ShiftOffset(core)
  cross_ambient_offset = CrossAmbientOffset(core)
  single_damper = SingleDamper(core)
  user_setpoint = UserSetpoint(core)
  user_setpoint_data = UserSetpointData(core)
  damper_heater = DamperHeater(core)
  pulldown = Pulldown(core)
  aluminum_mold_ice_maker = AluminumMoldIceMaker(core)
  freezer_ice_rate = FreezerIceRate(core)
  ice_maker_fill_monitor = IceMakerFillMonitor(core)
  ice_maker = IceMaker(core)
  fill_tube_heater = FillTubeHeater(core)
  flow_meter = FlowMeter(core)
  erd_list_to_sorted_erd_mappings = ErdListToSortedErdMappings(core)
  twist_tray_ice_maker = TwistTrayIceMaker(core)
  twist_tray_ice_maker_fill_monitor = IceMakerFillMonitor(core)
  dispenser = Dispenser(core)
  auger_motor = AugerMotor(core)
  variable_sweat_heater = VariableSweatHeater(core)
  recess_heater = RecessHeater(core)
  condenser_fan = CondenserFan(core)

  return {
    core = core,
    parametric_data = parametric_data,
    compressor = compressor,
    compressor_speeds = compressorSpeeds,
    compressor_times = compressorTimes,
    cooling_mode_independent_compressor_speeds = coolingModeIndependentCompressorSpeeds,
    cooling_mode_dependent_compressor_speeds = coolingModeDependentCompressorSpeeds,
    convertibleCompartment = convertibleCompartment,
    defrost = defrost,
    duty_cycle = duty_cycle,
    door_inhibit_dispense_table = door_inhibit_dispense_table,
    enhancedSabbath = enhancedSabbath,
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
    evaporator = evaporator,
    sensors = sensors,
    sensor_type = sensor_type,
    convertible_compartment_sensor_type = convertible_compartment_sensor_type,
    systemMonitor = systemMonitor,
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
    ice_box_adjusted_setpoint = ice_box_adjusted_setpoint,
    cross_ambient_offset_calculator = cross_ambient_offset_calculator,
    shift_offset_calculator = shift_offset_calculator,
    fixed_setpoint_offset = fixed_setpoint_offset,
    setpoint_offset = setpoint_offset,
    shift_offset = shift_offset,
    cross_ambient_offset = cross_ambient_offset,
    single_damper = single_damper,
    user_setpoint = user_setpoint,
    user_setpoint_data = user_setpoint_data,
    damper_heater = damper_heater,
    pulldown = pulldown,
    aluminum_mold_ice_maker = aluminum_mold_ice_maker,
    freezer_ice_rate = freezer_ice_rate,
    ice_maker_fill_monitor = ice_maker_fill_monitor,
    ice_maker = ice_maker,
    fill_tube_heater = fill_tube_heater,
    flow_meter = flow_meter,
    twist_tray_ice_maker = twist_tray_ice_maker,
    twist_tray_ice_maker_fill_monitor = twist_tray_ice_maker_fill_monitor,
    dispenser = dispenser,
    auger_motor = auger_motor,
    variable_sweat_heater = variable_sweat_heater,
    recess_heater = recess_heater,
    condenser_fan = condenser_fan,
    enum = enum,
    erd_list_to_sorted_erd_mappings = erd_list_to_sorted_erd_mappings,
    import = require 'lua-parametric-tools'.util.EvalInCurrentEnvironment(directory, {
      remove_tail_calls = true
    })
  }
end
