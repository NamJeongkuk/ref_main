/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ExternalDataSource.h"
#include "SystemErds.h"
#include "DeltaGridLines.h"

#define EXPAND_AS_MAPPED_ERD_PAIR(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_RAM_, StorageType)                                                                          \
   ({ Number COMMA Name } COMMA)

static const DataSource_MappedErdPair_t internalExternalMappings[] = {
   ERD_TABLE(EXPAND_AS_MAPPED_ERD_PAIR)
};

static const ConstArrayMap_DirectIndexConfiguration_t internalToExternalMapConfiguration = {
   internalExternalMappings,
   ELEMENT_SIZE(internalExternalMappings),
   NUM_ELEMENTS(internalExternalMappings),
   RamErdBase
};

static const ConstArrayMap_BinarySearchConfiguration_t externalToInternalMapConfiguration = {
   internalExternalMappings,
   NUM_ELEMENTS(internalExternalMappings),
   ELEMENT_SIZE(internalExternalMappings),
   MEMBER_SIZE(DataSource_MappedErdPair_t, mappedErdId),
   OFFSET_OF(DataSource_MappedErdPair_t, mappedErdId),
   IS_SIGNED(Erd_t)
};

// Entries must be ordered by public ERD
static const DataSource_EndiannessSwappedSwappedField_t swappedFields[] = {
   // 0x0000 range
   SWAPPED_ERD(PublicErd_BuildNumber, sizeof(uint32_t)),
   SWAPPED_ERD(PublicErd_AppliancePersonality, sizeof(AppliancePersonality_t)),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsRevision3Manifest, ServiceDiagnosticsRevision3Manifest_t, entityMapStart),
   SWAPPED_FIELD(PublicErd_ApplianceApiManifest, ApplianceApiManifest_t, revision),
   SWAPPED_FIELD(PublicErd_ApplianceApiManifest, ApplianceApiManifest_t, features),
   SWAPPED_ERD(PublicErd_ResetCount, sizeof(ResetCount_t)),
   SWAPPED_ERD(PublicErd_SecondsSinceLastReset, sizeof(SecondsSinceLastReset_t)),
   SWAPPED_ERD(PublicErd_ProgramCounterAddressAtLastUassert, sizeof(ProgramCounterAddress_t)),

   // 0xE000 range
   SWAPPED_ERD(PublicErd_ServiceDiagnosticsEntityLocation, sizeof(Erd_t)),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsEntityManifest, ServiceDiagnosticsRevision3EntityManifest_t, featureBitmap),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsEntityManifest, ServiceDiagnosticsRevision3EntityManifest_t, entryCount.faultTable),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsEntityManifest, ServiceDiagnosticsRevision3EntityManifest_t, entryCount.faultSequence),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsEntityManifest, ServiceDiagnosticsRevision3EntityManifest_t, entryCount.cycleHistorySequence),
   SWAPPED_ERD(PublicErd_ServiceDiagnosticsEntityCycleCount, sizeof(uint32_t)),
   SWAPPED_ERD(PublicErd_ServiceDiagnosticsRunTimeInMinutes, sizeof(ApplianceRunTimeMinutes_t)),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsFaultTableStatus, ServiceDiagnosticsRevision3TableStatus_t, currentIndex),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsFaultTableStatus, ServiceDiagnosticsRevision3TableStatus_t, count),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsFaultTableStatus, ServiceDiagnosticsRevision3TableStatus_t, overwriteCount),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsFaultTableStatus, ServiceDiagnosticsRevision3TableStatus_t, reserved),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsFaultTableEntry0, ServiceDiagnosticsRevision3FaultTableEntry_t, timeStamp),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsFaultTableEntry0, ServiceDiagnosticsRevision3FaultTableEntry_t, faultCode),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsFaultTableEntry0, ServiceDiagnosticsRevision3FaultTableEntry_t, faultCount),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsFaultTableSnapshot0, FaultSnapshot_t, header.cycleNumber),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsFaultTableSnapshot0, FaultSnapshot_t, header.timeStamp),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsFaultTableSnapshot0, FaultSnapshot_t, header.faultCode),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsFaultSequenceStatus, ServiceDiagnosticsRevision3SequenceStatus_t, currentIndex),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsFaultSequenceStatus, ServiceDiagnosticsRevision3SequenceStatus_t, count),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsFaultSequenceStatus, ServiceDiagnosticsRevision3SequenceStatus_t, overwriteCount),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsFaultSequenceStatus, ServiceDiagnosticsRevision3SequenceStatus_t, reserved),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsFaultSequenceEntry0, ServiceDiagnosticsRevision3FaultSequenceEntry_t, cycleNumber),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsFaultSequenceEntry0, ServiceDiagnosticsRevision3FaultSequenceEntry_t, timeStamp),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsFaultSequenceEntry0, ServiceDiagnosticsRevision3FaultSequenceEntry_t, faultCode),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsCycleHistorySequenceStatus, ServiceDiagnosticsRevision3SequenceStatus_t, currentIndex),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsCycleHistorySequenceStatus, ServiceDiagnosticsRevision3SequenceStatus_t, count),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsCycleHistorySequenceStatus, ServiceDiagnosticsRevision3SequenceStatus_t, overwriteCount),
   SWAPPED_FIELD(PublicErd_DiagnosticsCycleHistoryRecord0, CycleHistoryRecord_t, someData),
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsCycleHistoryAddEntry, CycleHistoryRecord_t, someData),

   // 0xF000 range
   SWAPPED_FIELD(PublicErd_TimerModuleDiagnosticsResult, TimerModuleDiagnosticsResults_t, maximumLatency),
   SWAPPED_FIELD(PublicErd_TimerModuleDiagnosticsResult, TimerModuleDiagnosticsResults_t, averageLatency),
   SWAPPED_ERD(PublicErd_ParametricDataImageCrc, sizeof(uint16_t)),
   SWAPPED_ERD(PublicErd_SomeData, sizeof(uint32_t)),
   SWAPPED_ERD(PublicErd_ApplianceRunTimeInMinutesUpdatedHourly, sizeof(ApplianceRunTimeMinutes_t)),
   SWAPPED_ERD(PublicErd_FreshFood_LongTermAverageInDegFx100, sizeof(TemperatureDegFx100_t)),

   // 0xF100 range
   SWAPPED_ERD(PublicErd_FreezerDefrostCycleCount, sizeof(uint16_t)),
   SWAPPED_ERD(PublicErd_NumberOfFreezerAbnormalDefrostCycles, sizeof(uint16_t)),
   SWAPPED_ERD(PublicErd_DefrostFreezerScaledDoorAccelerationInSeconds, sizeof(uint32_t)),
   SWAPPED_ERD(PublicErd_DefrostFreshFoodScaledDoorAccelerationInSeconds, sizeof(uint32_t)),
   SWAPPED_ERD(PublicErd_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds, sizeof(uint32_t)),
   SWAPPED_ERD(PublicErd_MaxTimeBetweenDefrostsInMinutes, sizeof(uint16_t)),
   SWAPPED_ERD(PublicErd_DefrostPrechillRunCounterInMinutes, sizeof(uint16_t)),
   SWAPPED_ERD(PublicErd_DefrostCompressorOnTimeInSeconds, sizeof(uint32_t)),
   SWAPPED_ERD(PublicErd_TimeInMinutesUntilReadyToDefrost, sizeof(uint16_t)),
   SWAPPED_ERD(PublicErd_TimeThatPrechillConditionsAreMetInMinutes, sizeof(uint16_t)),
   SWAPPED_ERD(PublicErd_AluminumMoldIceMakerMoldThermistor_AdcCount, sizeof(AdcCounts_t)),
   SWAPPED_ERD(PublicErd_FreshFood_UnfilteredTemperatureInDegFx100, sizeof(TemperatureDegFx100_t)),
   SWAPPED_FIELD(PublicErd_ConvertibleCompartmentSetpoint_ResolvedVote, SetpointVotedTemperature_t, temperatureInDegFx100),
   SWAPPED_ERD(PublicErd_ConvertibleCompartmentSetpoint_WinningVoteErd, sizeof(WinningVoteErd_t)),
   SWAPPED_FIELD(PublicErd_ConvertibleCompartmentSetpoint_FactoryVote, SetpointVotedTemperature_t, temperatureInDegFx100),
   SWAPPED_FIELD(PublicErd_ConvertibleCompartmentSetpoint_UserVote, SetpointVotedTemperature_t, temperatureInDegFx100),
   SWAPPED_ERD(PublicErd_TimeInMinutesInValvePositionB, sizeof(uint32_t)),
   SWAPPED_ERD(PublicErd_FreezerEvap_FilteredTemperatureOverrideValueInDegFx100, sizeof(TemperatureDegFx100_t)),
   SWAPPED_ERD(PublicErd_Freezer_FilteredTemperatureOverrideValueInDegFx100, sizeof(TemperatureDegFx100_t)),
   SWAPPED_ERD(PublicErd_FreshFood_FilteredTemperatureOverrideValueInDegFx100, sizeof(TemperatureDegFx100_t)),
   SWAPPED_FIELD(PublicErd_CalculatedCondenserFanControl, FanControl_t, rpm),
   SWAPPED_FIELD(PublicErd_CalculatedIceCabinetFanControl, FanControl_t, rpm),
   SWAPPED_FIELD(PublicErd_CalculatedConvertibleCompartmentFanControl, FanControl_t, rpm),
   SWAPPED_FIELD(PublicErd_CalculatedDeliFanControl, FanControl_t, rpm),
   SWAPPED_FIELD(PublicErd_CalculatedFreezerEvapFanControl, FanControl_t, rpm),
   SWAPPED_FIELD(PublicErd_CalculatedFreshFoodEvapFanControl, FanControl_t, rpm),
   SWAPPED_FIELD(PublicErd_FreezerSetpoint_TemperatureBounds, SetpointZoneTemperatureBounds_t, coldSetpointBoundInDegFx100),
   SWAPPED_FIELD(PublicErd_FreezerSetpoint_TemperatureBounds, SetpointZoneTemperatureBounds_t, warmSetpointBoundInDegFx100),
   SWAPPED_ERD(PublicErd_TimeAcceleration_Ticks, sizeof(uint32_t)),
   SWAPPED_ERD(PublicErd_Ambient_FilteredTemperatureOverrideValueInDegFx100, sizeof(TemperatureDegFx100_t)),
   SWAPPED_ERD(PublicErd_AluminumMoldIceMaker_FilteredTemperatureOverrideValueInDegFx100, sizeof(TemperatureDegFx100_t)),
   SWAPPED_FIELD(PublicErd_AluminumMoldIceMakerMoldHeaterControlRequest, IceMakerMoldHeaterControlRequest_t, onTemperatureInDegFx100),
   SWAPPED_FIELD(PublicErd_AluminumMoldIceMakerMoldHeaterControlRequest, IceMakerMoldHeaterControlRequest_t, offTemperatureInDegFx100),

   // 0xF200 range
   SWAPPED_ERD(PublicErd_CompressorSpeed_WinningVoteErd, sizeof(WinningVoteErd_t)),
   SWAPPED_ERD(PublicErd_AluminumMoldIceMakerHeaterRelay_WinningVoteErd, sizeof(WinningVoteErd_t)),
   SWAPPED_ERD(PublicErd_CondenserFanSpeed_WinningVoteErd, sizeof(WinningVoteErd_t)),
   SWAPPED_ERD(PublicErd_IceCabinetFanSpeed_WinningVoteErd, sizeof(WinningVoteErd_t)),
   SWAPPED_ERD(PublicErd_ValvePosition_WinningVoteErd, sizeof(WinningVoteErd_t)),
   SWAPPED_ERD(PublicErd_FreshFoodEvapFanSpeed_WinningVoteErd, sizeof(WinningVoteErd_t)),
   SWAPPED_ERD(PublicErd_FreezerEvapFanSpeed_WinningVoteErd, sizeof(WinningVoteErd_t)),
   SWAPPED_ERD(PublicErd_WaterValve_WinningVoteErd, sizeof(WinningVoteErd_t)),
   SWAPPED_FIELD(PublicErd_Grid_CalculatedGridLines, CalculatedGridLines_t, freshFoodGridLine.gridLinesDegFx100[GridLine_Nfl]),
   SWAPPED_FIELD(PublicErd_Grid_CalculatedGridLines, CalculatedGridLines_t, freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodLowHyst]),
   SWAPPED_FIELD(PublicErd_Grid_CalculatedGridLines, CalculatedGridLines_t, freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodLowHystDelta]),
   SWAPPED_FIELD(PublicErd_Grid_CalculatedGridLines, CalculatedGridLines_t, freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodHighHyst]),
   SWAPPED_FIELD(PublicErd_Grid_CalculatedGridLines, CalculatedGridLines_t, freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodExtraHigh]),
   SWAPPED_FIELD(PublicErd_Grid_CalculatedGridLines, CalculatedGridLines_t, freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSuperHigh]),
   SWAPPED_FIELD(PublicErd_Grid_CalculatedGridLines, CalculatedGridLines_t, freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst]),
   SWAPPED_FIELD(PublicErd_Grid_CalculatedGridLines, CalculatedGridLines_t, freezerGridLine.gridLinesDegFx100[GridLine_FreezerDelta]),
   SWAPPED_FIELD(PublicErd_Grid_CalculatedGridLines, CalculatedGridLines_t, freezerGridLine.gridLinesDegFx100[GridLine_FreezerHighHyst]),
   SWAPPED_FIELD(PublicErd_Grid_CalculatedGridLines, CalculatedGridLines_t, freezerGridLine.gridLinesDegFx100[GridLine_FreezerExtraHigh]),
   SWAPPED_FIELD(PublicErd_Grid_CalculatedGridLines, CalculatedGridLines_t, freezerGridLine.gridLinesDegFx100[GridLine_FreezerSuperHigh]),
   SWAPPED_FIELD(PublicErd_Grid_CalculatedGridLines, CalculatedGridLines_t, freezerGridLine.gridLinesDegFx100[GridLine_FreezerExtremeHigh]),
   SWAPPED_ERD(PublicErd_FreshFood_AdjustedSetpointInDegFx100, sizeof(TemperatureDegFx100_t)),
   SWAPPED_FIELD(PublicErd_FreezerSetpoint_ResolvedVote, SetpointVotedTemperature_t, temperatureInDegFx100),
   SWAPPED_ERD(PublicErd_FreezerSetpoint_WinningVoteErd, sizeof(WinningVoteErd_t)),
   SWAPPED_FIELD(PublicErd_FreezerSetpoint_FactoryVote, SetpointVotedTemperature_t, temperatureInDegFx100),
   SWAPPED_FIELD(PublicErd_FreezerSetpoint_EnhancedSabbathVote, SetpointVotedTemperature_t, temperatureInDegFx100),
   SWAPPED_FIELD(PublicErd_FreezerSetpoint_TurboFreezeVote, SetpointVotedTemperature_t, temperatureInDegFx100),
   SWAPPED_FIELD(PublicErd_FreezerSetpoint_IceInDoorVote, SetpointVotedTemperature_t, temperatureInDegFx100),
   SWAPPED_FIELD(PublicErd_FreezerSetpoint_FreezerIceMakerVote, SetpointVotedTemperature_t, temperatureInDegFx100),
   SWAPPED_FIELD(PublicErd_FreezerSetpoint_UserVote, SetpointVotedTemperature_t, temperatureInDegFx100),
   SWAPPED_FIELD(PublicErd_FreshFoodSetpoint_ResolvedVote, SetpointVotedTemperature_t, temperatureInDegFx100),
   SWAPPED_ERD(PublicErd_FreshFoodSetpoint_WinningVoteErd, sizeof(WinningVoteErd_t)),
   SWAPPED_FIELD(PublicErd_FreshFoodSetpoint_FactoryVote, SetpointVotedTemperature_t, temperatureInDegFx100),
   SWAPPED_FIELD(PublicErd_FreshFoodSetpoint_EnhancedSabbathVote, SetpointVotedTemperature_t, temperatureInDegFx100),
   SWAPPED_FIELD(PublicErd_FreshFoodSetpoint_TurboCoolVote, SetpointVotedTemperature_t, temperatureInDegFx100),
   SWAPPED_FIELD(PublicErd_FreshFoodSetpoint_UserVote, SetpointVotedTemperature_t, temperatureInDegFx100),
   SWAPPED_ERD(PublicErd_FreshFoodDefrostHeater_WinningVoteErd, sizeof(WinningVoteErd_t)),
   SWAPPED_ERD(PublicErd_FreezerDefrostHeater_WinningVoteErd, sizeof(WinningVoteErd_t)),

   // 0xF300 range
   SWAPPED_ERD(PublicErd_ConvertibleCompartmentFanSpeed_WinningVoteErd, sizeof(WinningVoteErd_t)),
   SWAPPED_ERD(PublicErd_AluminumMoldIceMakerMotor_WinningVoteErd, sizeof(WinningVoteErd_t)),
   SWAPPED_ERD(PublicErd_DeliFanSpeed_WinningVoteErd, sizeof(WinningVoteErd_t)),
   SWAPPED_ERD(PublicErd_CondenserFan_Pwm, sizeof(PwmDutyCycle_t)),
   SWAPPED_ERD(PublicErd_CondenserFan_ActualRpm, sizeof(FanRpm_t)),
   SWAPPED_ERD(PublicErd_CondenserFan_InputCaptureTime, sizeof(InputCaptureMicroSeconds_t)),
   SWAPPED_ERD(PublicErd_FreshFoodDamperPosition_WinningVoteErd, sizeof(WinningVoteErd_t)),
   SWAPPED_ERD(PublicErd_AluminumMoldIceMakerFillTubeHeater_WinningVoteErd, sizeof(WinningVoteErd_t)),
   SWAPPED_ERD(PublicErd_FreshFood_ResolvedSetpointInDegFx100, sizeof(TemperatureDegFx100_t)),
   SWAPPED_FIELD(PublicErd_FreshFoodDamperStepperMotorPositionRequest, StepperPositionRequest_t, stepsToMove),
   SWAPPED_ERD(PublicErd_AluminumMoldFlowMeterWaterDispensedOzX100, sizeof(uint32_t)),
   SWAPPED_ERD(PublicErd_FreshFoodDamperHeater_WinningVoteErd, sizeof(WinningVoteErd_t)),
   SWAPPED_ERD(PublicErd_FillTubeHeater_Pwm, sizeof(PwmDutyCycle_t)),
   // 0xF400 range
   SWAPPED_ERD(PublicErd_ApplicationToBspPwm_PWM_0, sizeof(PwmDutyCycle_t)),
   SWAPPED_ERD(PublicErd_ApplicationToBspInputCapture_CAPT_0, sizeof(InputCaptureMicroSeconds_t)),
   SWAPPED_ERD(PublicErd_Adc_ADC_0, sizeof(AdcCounts_t)),

   // 0xF500 range
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsFaultTableSnapshotData0, FaultSnapshotData_t, someData),

   // 0xFB00 range
   SWAPPED_ERD(PublicErd_Eeprom_DefrostCompressorOnTimeInSeconds, sizeof(uint32_t))
};

static const DataSource_EndiannessSwappedSwappedFieldRange_t swappedRanges[] = {
   SWAPPED_FIELD_RANGE(PublicErd_FreshFood_LongTermAverageInDegFx100, PublicErd_DeliPan_LongTermAverageInDegFx100),
   SWAPPED_FIELD_RANGE(PublicErd_CondenserFan_Pwm, PublicErd_FreshFoodEvapFan_Pwm),
   SWAPPED_FIELD_RANGE(PublicErd_CondenserFan_ActualRpm, PublicErd_FreshFoodEvapFan_ActualRpm),
   SWAPPED_FIELD_RANGE(PublicErd_CondenserFan_InputCaptureTime, PublicErd_AluminumIceMakerRake_InputCaptureTime),
   SWAPPED_FIELD_RANGE(PublicErd_FreshFood_ResolvedSetpointInDegFx100, PublicErd_DeliPan_ThermalShiftInDegFx100),
   SWAPPED_FIELD_RANGE(PublicErd_FillTubeHeater_Pwm, PublicErd_FreezerTopLight_Pwm),
   SWAPPED_FIELD_RANGE(PublicErd_ServiceDiagnosticsFaultTableEntry0, PublicErd_ServiceDiagnosticsFaultTableEntry9),
   SWAPPED_FIELD_RANGE(PublicErd_ServiceDiagnosticsFaultTableSnapshot0, PublicErd_ServiceDiagnosticsFaultTableSnapshot9),
   SWAPPED_FIELD_RANGE(PublicErd_ServiceDiagnosticsFaultSequenceEntry0, PublicErd_ServiceDiagnosticsFaultSequenceEntry7),
   SWAPPED_FIELD_RANGE(PublicErd_DiagnosticsCycleHistoryRecord0, PublicErd_DiagnosticsCycleHistoryRecord9),
   SWAPPED_FIELD_RANGE(PublicErd_AluminumMoldIceMakerMoldThermistor_AdcCount, PublicErd_ConvertibleCompartmentEvapThermistor_AdcCount),
   SWAPPED_FIELD_RANGE(PublicErd_FreshFood_UnfilteredTemperatureInDegFx100, PublicErd_AluminumMoldIceMakerMold_FilteredTemperatureInDegFx100),
   SWAPPED_FIELD_RANGE(PublicErd_FreshFood_AdjustedSetpointInDegFx100, PublicErd_DeliPan_AdjustedSetpointInDegFx100),
   SWAPPED_FIELD_RANGE(PublicErd_ConvertibleCompartmentSetpoint_FactoryVote, PublicErd_ConvertibleCompartmentSetpoint_UserVote),
   SWAPPED_FIELD_RANGE(PublicErd_FreezerEvap_FilteredTemperatureOverrideValueInDegFx100, PublicErd_FreezerEvap_FilteredTemperatureResolvedInDegFx100),
   SWAPPED_FIELD_RANGE(PublicErd_Freezer_FilteredTemperatureOverrideValueInDegFx100, PublicErd_Freezer_FilteredTemperatureResolvedInDegFx100),
   SWAPPED_FIELD_RANGE(PublicErd_FreshFood_FilteredTemperatureOverrideValueInDegFx100, PublicErd_FreshFood_FilteredTemperatureResolvedInDegFx100),
   SWAPPED_FIELD_RANGE(PublicErd_FreezerSetpoint_TemperatureBounds, PublicErd_ConvertibleCompartmentSetpoint_TemperatureBounds),
   SWAPPED_FIELD_RANGE(PublicErd_Ambient_FilteredTemperatureOverrideValueInDegFx100, PublicErd_Ambient_FilteredTemperatureResolvedInDegFx100),
   SWAPPED_FIELD_RANGE(PublicErd_AluminumMoldIceMaker_FilteredTemperatureOverrideValueInDegFx100, PublicErd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100),
   SWAPPED_FIELD_RANGE(PublicErd_FreezerSetpoint_FactoryVote, PublicErd_FreezerSetpoint_UserVote),
   SWAPPED_FIELD_RANGE(PublicErd_FreshFoodSetpoint_FactoryVote, PublicErd_FreshFoodSetpoint_UserVote),
   SWAPPED_FIELD_RANGE(PublicErd_ApplicationToBspPwm_PWM_0, PublicErd_Pwm_PWM_200_03),
   SWAPPED_FIELD_RANGE(PublicErd_ApplicationToBspInputCapture_CAPT_0, PublicErd_InputCapture_CAPTURE_05),
   SWAPPED_FIELD_RANGE(PublicErd_Adc_ADC_0, PublicErd_Adc_HW_PERSONALITY_01),
   SWAPPED_FIELD_RANGE(PublicErd_ServiceDiagnosticsFaultTableSnapshotData0, PublicErd_ServiceDiagnosticsFaultTableSnapshotData9),
   SWAPPED_FIELD_RANGE(PublicErd_Eeprom_DefrostCompressorOnTimeInSeconds, PublicErd_Eeprom_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds),
   SWAPPED_FIELD_RANGE(PublicErd_FillTubeHeater_Pwm, PublicErd_FreshFoodDamperHeaterPwmDutyCycle),
};

static const DataSource_EndiannessSwappedSwappedRangesConfiguration_t endianessSwappedSwappedRangesConfiguration = {
   swappedRanges,
   NUM_ELEMENTS(swappedRanges)
};

static const ConstMultiMap_BinarySearchConfiguration_t endiannessSwappedFieldsMapConfiguration = {
   swappedFields,
   NUM_ELEMENTS(swappedFields),
   sizeof(DataSource_EndiannessSwappedSwappedField_t),
   MEMBER_SIZE(DataSource_EndiannessSwappedSwappedField_t, erd),
   OFFSET_OF(DataSource_EndiannessSwappedSwappedField_t, erd),
   sizeof(DataSource_EndiannessSwappedSwappedField_t),
   0,
   IS_SIGNED(Erd_t)
};

void ExternalDataSource_Init(
   ExternalDataSource_t *instance,
   I_DataSource_t *dataSource)
{
   ConstArrayMap_DirectIndex_Init(&instance->_private.internalToExternalMap, &internalToExternalMapConfiguration);
   ConstArrayMap_BinarySearch_Init(&instance->_private.externalToInternalMap, &externalToInternalMapConfiguration);
   ConstBidirectionalMap_ConstArrayMap_Init(
      &instance->_private.internalExternalMap,
      &instance->_private.externalToInternalMap.interface,
      &instance->_private.internalToExternalMap.interface);

   ConstMultiMap_BinarySearch_Init(&instance->_private.swappedFieldsMap, &endiannessSwappedFieldsMapConfiguration);

   DataSource_Mapped_InitWithPassThrough(
      &instance->_private.mappedDataSource,
      dataSource,
      &instance->_private.internalExternalMap.interface);

   DataSource_EndiannessSwapped_Init(
      &instance->_private.endiannessSwappedDataSource,
      &instance->_private.mappedDataSource.interface,
      &instance->_private.swappedFieldsMap.interface,
      &endianessSwappedSwappedRangesConfiguration,
      false);
}

I_DataSource_t *ExternalDataSource_DataSource(ExternalDataSource_t *instance)
{
   return &instance->_private.endiannessSwappedDataSource.interface;
}
