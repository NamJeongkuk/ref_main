/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ExternalDataSource.h"
#include "SystemErds.h"

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
   SWAPPED_ERD(PublicErd_Eeprom_DefrostTimerCountInSeconds, sizeof(uint32_t)),
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
   SWAPPED_ERD(PublicErd_SomeData, sizeof(uint32_t)),
   SWAPPED_ERD(PublicErd_ApplianceRunTimeInMinutesUpdatedHourly, sizeof(ApplianceRunTimeMinutes_t)),

   // 0xF100 range
   SWAPPED_ERD(PublicErd_FzAbnormalDefrostCycleCount, sizeof(uint16_t)),
   SWAPPED_ERD(PublicErd_FzDefrostCycleCount, sizeof(uint16_t)),
   SWAPPED_ERD(PublicErd_NumberofFzAbnormalDefrostCycles, sizeof(uint16_t)),
   SWAPPED_ERD(PublicErd_DefrostFzDoorAccelerationCount, sizeof(uint32_t)),
   SWAPPED_ERD(PublicErd_DefrostFfDoorAccelerationCount, sizeof(uint32_t)),
   SWAPPED_ERD(PublicErd_DefrostTimerCountInSeconds, sizeof(uint32_t)),
   SWAPPED_ERD(PublicErd_MaxTimeBetweenDefrostsInMinutes, sizeof(uint16_t)),
   SWAPPED_ERD(PublicErd_FreshFoodThermistor_AdcCount, sizeof(AdcCounts_t)),
   SWAPPED_ERD(PublicErd_FreshFood_UnfilteredTemperature, sizeof(TemperatureDegFx100_t)),

   // 0xF200 range
   SWAPPED_ERD(PublicErd_SomeAnalogInput, sizeof(uint16_t)),
   SWAPPED_ERD(PublicErd_AnotherAnalogInput, sizeof(uint16_t)),
   SWAPPED_ERD(PublicErd_FreshFood_AdjustedSetpoint, sizeof(TemperatureDegFx100_t)),

   // 0xF500 range
   SWAPPED_FIELD(PublicErd_ServiceDiagnosticsFaultTableSnapshotData0, FaultSnapshotData_t, someData)
};

static const DataSource_EndiannessSwappedSwappedFieldRange_t swappedRanges[] = {
   SWAPPED_FIELD_RANGE(PublicErd_ServiceDiagnosticsFaultTableEntry0, PublicErd_ServiceDiagnosticsFaultTableEntry9),
   SWAPPED_FIELD_RANGE(PublicErd_ServiceDiagnosticsFaultTableSnapshot0, PublicErd_ServiceDiagnosticsFaultTableSnapshot9),
   SWAPPED_FIELD_RANGE(PublicErd_ServiceDiagnosticsFaultSequenceEntry0, PublicErd_ServiceDiagnosticsFaultSequenceEntry7),
   SWAPPED_FIELD_RANGE(PublicErd_DiagnosticsCycleHistoryRecord0, PublicErd_DiagnosticsCycleHistoryRecord9),
   SWAPPED_FIELD_RANGE(PublicErd_FreshFoodThermistor_AdcCount, PublicErd_FreezerThermistor_AdcCount),
   SWAPPED_FIELD_RANGE(PublicErd_FreshFood_UnfilteredTemperature, PublicErd_Freezer_FilteredTemperature),
   SWAPPED_FIELD_RANGE(PublicErd_FreshFood_AdjustedSetpoint, PublicErd_Freezer_Shift),
   SWAPPED_FIELD_RANGE(PublicErd_ServiceDiagnosticsFaultTableSnapshotData0, PublicErd_ServiceDiagnosticsFaultTableSnapshotData9),
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
