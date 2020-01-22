/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ExternalDataSource.h"
#include "SystemErds.h"

#define EXPAND_AS_MAPPED_ERD_PAIR(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_RAM_, StorageType)                                                         \
   ({ Number COMMA Name } COMMA)

static const DataSource_MappedErdPair_t internalExternalMappings[] =
   {
      ERD_TABLE(EXPAND_AS_MAPPED_ERD_PAIR)
   };

static const ConstArrayMap_DirectIndexConfiguration_t internalToExternalMapConfiguration =
   {
      internalExternalMappings,
      ELEMENT_SIZE(internalExternalMappings),
      NUM_ELEMENTS(internalExternalMappings),
      ErdBaseId
   };

static const ConstArrayMap_BinarySearchConfiguration_t externalToInternalMapConfiguration =
   {
      internalExternalMappings,
      NUM_ELEMENTS(internalExternalMappings),
      ELEMENT_SIZE(internalExternalMappings),
      MEMBER_SIZE(DataSource_MappedErdPair_t, mappedErdId),
      OFFSET_OF(DataSource_MappedErdPair_t, mappedErdId),
      IS_SIGNED(Erd_t)
   };

static const DataSource_EndiannessSwappedSwappedField_t swappedFields[] =
   {
      // NV ERDs
      SWAPPED_ERD(Erd_ServiceDiagnosticsEntityCycleCount, sizeof(uint32_t)),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultTableStatus, ServiceDiagnosticsRevision3TableStatus_t, currentIndex),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultTableStatus, ServiceDiagnosticsRevision3TableStatus_t, count),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultTableStatus, ServiceDiagnosticsRevision3TableStatus_t, overwriteCount),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultTableStatus, ServiceDiagnosticsRevision3TableStatus_t, reserved),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultTableEntry0, ServiceDiagnosticsRevision3FaultTableEntry_t, timeStamp),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultTableEntry0, ServiceDiagnosticsRevision3FaultTableEntry_t, faultCode),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultTableEntry0, ServiceDiagnosticsRevision3FaultTableEntry_t, faultCount),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultTableSnapshot0, FaultSnapshot_t, header.cycleNumber),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultTableSnapshot0, FaultSnapshot_t, header.timeStamp),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultTableSnapshot0, FaultSnapshot_t, header.faultCode),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultSequenceStatus, ServiceDiagnosticsRevision3SequenceStatus_t, currentIndex),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultSequenceStatus, ServiceDiagnosticsRevision3SequenceStatus_t, count),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultSequenceStatus, ServiceDiagnosticsRevision3SequenceStatus_t, overwriteCount),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultSequenceStatus, ServiceDiagnosticsRevision3SequenceStatus_t, reserved),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultSequenceEntry00, ServiceDiagnosticsRevision3FaultSequenceEntry_t, cycleNumber),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultSequenceEntry00, ServiceDiagnosticsRevision3FaultSequenceEntry_t, timeStamp),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultSequenceEntry00, ServiceDiagnosticsRevision3FaultSequenceEntry_t, faultCode),
      SWAPPED_FIELD(Erd_DiagnosticsCycleHistoryRecord0, CycleHistoryRecord_t, someData),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultTableSnapshotData0, FaultSnapshotData_t, someData),

      // Other ERDs
      SWAPPED_ERD(Erd_BuildNumber, sizeof(uint32_t)),
      SWAPPED_ERD(Erd_AppliancePersonality, sizeof(AppliancePersonality_t)),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsRevision3Manifest, ServiceDiagnosticsRevision3Manifest_t, entityMapStart),
      SWAPPED_FIELD(Erd_ApplianceApiManifest, ApplianceApiManifest_t, revision),
      SWAPPED_FIELD(Erd_ApplianceApiManifest, ApplianceApiManifest_t, features),
      SWAPPED_ERD(Erd_ServiceDiagnosticsEntityLocation, sizeof(Erd_t)),
      SWAPPED_ERD(Erd_ServiceDiagnosticsRunTimeInMinutes, sizeof(ApplianceRunTimeMinutes_t)),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsEntityManifest, ServiceDiagnosticsRevision3EntityManifest_t, featureBitmap),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsEntityManifest, ServiceDiagnosticsRevision3EntityManifest_t, entryCount.faultTable),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsEntityManifest, ServiceDiagnosticsRevision3EntityManifest_t, entryCount.faultSequence),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsEntityManifest, ServiceDiagnosticsRevision3EntityManifest_t, entryCount.cycleHistorySequence),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultTableStatus, ServiceDiagnosticsRevision3TableStatus_t, currentIndex),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultTableStatus, ServiceDiagnosticsRevision3TableStatus_t, count),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultTableStatus, ServiceDiagnosticsRevision3TableStatus_t, overwriteCount),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultSequenceStatus, ServiceDiagnosticsRevision3SequenceStatus_t, currentIndex),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultSequenceStatus, ServiceDiagnosticsRevision3SequenceStatus_t, count),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultSequenceStatus, ServiceDiagnosticsRevision3SequenceStatus_t, overwriteCount),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsCycleHistorySequenceStatus, ServiceDiagnosticsRevision3SequenceStatus_t, currentIndex),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsCycleHistorySequenceStatus, ServiceDiagnosticsRevision3SequenceStatus_t, count),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsCycleHistorySequenceStatus, ServiceDiagnosticsRevision3SequenceStatus_t, overwriteCount),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsCycleHistoryAddEntry, CycleHistoryRecord_t, someData),
      SWAPPED_FIELD(Erd_TimerModuleDiagnosticsResult, TimerModuleDiagnosticsResults_t, maximumLatency),
      SWAPPED_FIELD(Erd_TimerModuleDiagnosticsResult, TimerModuleDiagnosticsResults_t, averageLatency),
      SWAPPED_ERD(Erd_SomeData, sizeof(uint32_t)),
      SWAPPED_ERD(Erd_ApplianceRunTimeInMinutesUpdatedHourly, sizeof(uint32_t)),
      SWAPPED_ERD(Erd_SomeAnalogInput, sizeof(uint16_t)),
      SWAPPED_ERD(Erd_AnotherAnalogInput, sizeof(uint16_t))
   };

static const DataSource_EndiannessSwappedSwappedFieldRange_t swappedRanges[] =
   {
      SWAPPED_FIELD_RANGE(Erd_ServiceDiagnosticsFaultTableEntry0, Erd_ServiceDiagnosticsFaultTableEntry9),
      SWAPPED_FIELD_RANGE(Erd_ServiceDiagnosticsFaultTableSnapshot0, Erd_ServiceDiagnosticsFaultTableSnapshot9),
      SWAPPED_FIELD_RANGE(Erd_ServiceDiagnosticsFaultSequenceEntry00, Erd_ServiceDiagnosticsFaultSequenceEntry07),
      SWAPPED_FIELD_RANGE(Erd_DiagnosticsCycleHistoryRecord0, Erd_DiagnosticsCycleHistoryRecord9),
      SWAPPED_FIELD_RANGE(Erd_ServiceDiagnosticsFaultTableSnapshotData0, Erd_ServiceDiagnosticsFaultTableSnapshotData9)
   };

static const DataSource_EndiannessSwappedSwappedRangesConfiguration_t endianessSwappedSwappedRangesConfiguration =
   {
      swappedRanges,
      NUM_ELEMENTS(swappedRanges)
   };

static const ConstMultiMap_LinearSearchConfiguration_t endiannessSwappedFieldsMapConfiguration =
   {
      swappedFields,
      NUM_ELEMENTS(swappedFields),
      sizeof(DataSource_EndiannessSwappedSwappedField_t),
      MEMBER_SIZE(DataSource_EndiannessSwappedSwappedField_t, erd),
      OFFSET_OF(DataSource_EndiannessSwappedSwappedField_t, erd),
      sizeof(DataSource_EndiannessSwappedSwappedField_t),
      0
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

   ConstMultiMap_LinearSearch_Init(&instance->_private.swappedFieldsMap, &endiannessSwappedFieldsMapConfiguration);

   DataSource_EndiannessSwapped_Init(
      &instance->_private.endiannessSwappedDataSource,
      dataSource,
      &instance->_private.swappedFieldsMap.interface,
      &endianessSwappedSwappedRangesConfiguration,
      false);

   DataSource_Mapped_InitWithPassThrough(
      &instance->_private.mappedDataSource,
      &instance->_private.endiannessSwappedDataSource.interface,
      &instance->_private.internalExternalMap.interface);
}

I_DataSource_t *ExternalDataSource_DataSource(ExternalDataSource_t *instance)
{
   return &instance->_private.mappedDataSource.interface;
}
