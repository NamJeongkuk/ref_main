/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ExternalDataSource.h"
#include "SystemErds.h"
#include "DeltaGridLines.h"
#include "StdintSwapDefinitions.h"

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

#define EXPAND_AS_SWAPPED_FIELDS(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_SWAP_, Swap)                                                                               \
   (CONCAT(SWAP_DEFINITION_, DataType)(Public##Name, DataType))

static const DataSource_EndiannessSwappedSwappedField_t swappedFields[] = {
   ERD_TABLE(EXPAND_AS_SWAPPED_FIELDS)
};

static const DataSource_EndiannessSwappedSwappedFieldRange_t swappedRanges[] = {
   SWAPPED_FIELD_RANGE(PublicErd_ServiceDiagnosticsFaultTableEntry0, PublicErd_ServiceDiagnosticsFaultTableEntry9),
   SWAPPED_FIELD_RANGE(PublicErd_ServiceDiagnosticsFaultTableSnapshot0, PublicErd_ServiceDiagnosticsFaultTableSnapshot9),
   SWAPPED_FIELD_RANGE(PublicErd_ServiceDiagnosticsFaultSequenceEntry0, PublicErd_ServiceDiagnosticsFaultSequenceEntry7),
   SWAPPED_FIELD_RANGE(PublicErd_DiagnosticsCycleHistoryRecord0, PublicErd_DiagnosticsCycleHistoryRecord9),
   SWAPPED_FIELD_RANGE(PublicErd_LeftHandFreshFoodScaledDoorAccelerationInSeconds, PublicErd_ConvertibleCompartmentAsFreezerScaledDoorAccelerationInSeconds),
   SWAPPED_FIELD_RANGE(PublicErd_TwistTrayIceMakerThermistor_AdcCount, PublicErd_ConvertibleCompartmentEvapThermistor_AdcCount),
   SWAPPED_FIELD_RANGE(PublicErd_FreshFood_UnfilteredTemperatureInDegFx100, PublicErd_TwistTrayIceMaker_UnfilteredTemperatureInDegFx100),
   SWAPPED_FIELD_RANGE(PublicErd_FreshFood_FilteredTemperatureInDegFx100, PublicErd_TwistTrayIceMaker_FilteredTemperatureInDegFx100),
   SWAPPED_FIELD_RANGE(PublicErd_FreezerSetpoint_TemperatureBounds, PublicErd_ConvertibleCompartmentSetpoint_TemperatureBounds),
   SWAPPED_FIELD_RANGE(PublicErd_FreshFood_AdjustedSetpointInDegFx100, PublicErd_Freezer_AdjustedSetpointInDegFx100),
   SWAPPED_FIELD_RANGE(PublicErd_FreshFood_ResolvedSetpointInDegFx100, PublicErd_Freezer_CabinetPlusCrossAmbientOffsetInDegFx100),
   SWAPPED_FIELD_RANGE(PublicErd_FreezerSetpoint_FactoryVote, PublicErd_FreezerSetpoint_UserVote),
   SWAPPED_FIELD_RANGE(PublicErd_FreshFoodSetpoint_FactoryVote, PublicErd_FreshFoodSetpoint_UserVote),
   SWAPPED_FIELD_RANGE(PublicErd_ConvertibleCompartmentSetpoint_FactoryVote, PublicErd_ConvertibleCompartmentSetpoint_UserVote),
   SWAPPED_FIELD_RANGE(PublicErd_FreezerEvap_FilteredTemperatureOverrideValueInDegFx100, PublicErd_FreezerEvap_FilteredTemperatureResolvedInDegFx100),
   SWAPPED_FIELD_RANGE(PublicErd_Freezer_FilteredTemperatureOverrideValueInDegFx100, PublicErd_Freezer_FilteredTemperatureResolvedInDegFx100),
   SWAPPED_FIELD_RANGE(PublicErd_FreshFood_FilteredTemperatureOverrideValueInDegFx100, PublicErd_FreshFood_FilteredTemperatureResolvedInDegFx100),
   SWAPPED_FIELD_RANGE(PublicErd_Ambient_FilteredInternalTemperatureOverrideValueInDegFx100, PublicErd_Ambient_FilteredInternalTemperatureResolvedInDegFx100),
   SWAPPED_FIELD_RANGE(PublicErd_AluminumMoldIceMaker_FilteredTemperatureOverrideValueInDegFx100, PublicErd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100),
   SWAPPED_FIELD_RANGE(PublicErd_CalculatedCondenserFanControl, PublicErd_CalculatedFreshFoodEvapFanControl),
   SWAPPED_FIELD_RANGE(PublicErd_CondenserFan_Pwm, PublicErd_FreshFoodEvapFan_Pwm),
   SWAPPED_FIELD_RANGE(PublicErd_CondenserFan_ActualRpm, PublicErd_FreshFoodEvapFan_ActualRpm),
   SWAPPED_FIELD_RANGE(PublicErd_CondenserFan_InputCaptureTime, PublicErd_FlowMeter_InputCaptureCount),
   SWAPPED_FIELD_RANGE(PublicErd_FillTubeHeater_Pwm, PublicErd_FreezerBackWallLight_Pwm),
   SWAPPED_FIELD_RANGE(PublicErd_Adc_ADC_0, PublicErd_Adc_HW_PERSONALITY_01),
   SWAPPED_FIELD_RANGE(PublicErd_ServiceDiagnosticsFaultTableSnapshotData0, PublicErd_ServiceDiagnosticsFaultTableSnapshotData9),
   SWAPPED_FIELD_RANGE(PublicErd_Eeprom_LeftHandFreshFoodScaledDoorAccelerationInSeconds, PublicErd_Eeprom_ConvertibleCompartmentAsFreezerScaledDoorAccelerationInSeconds),
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
