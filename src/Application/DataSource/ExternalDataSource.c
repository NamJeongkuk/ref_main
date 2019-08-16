/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ExternalDataSource.h"
#include "SystemErds.h"

#define EXPAND_AS_MAPPED_RAM_ERD_PAIR(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_RAM_, StorageType)({ Number COMMA Name } COMMA)

static const DataSource_MappedErdPair_t internalExternalMappings[] =
   {
      ERD_TABLE(EXPAND_AS_MAPPED_RAM_ERD_PAIR)
   };

static const ConstArrayMap_DirectIndexConfiguration_t internalToExternalMapConfiguration =
   {
      internalExternalMappings,
      ELEMENT_SIZE(internalExternalMappings),
      NUM_ELEMENTS(internalExternalMappings),
      RamErdBaseId
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
      SWAPPED_ERD(Erd_AppliancePersonality, sizeof(AppliancePersonality_t)),

      // Virtual ERDs
      SWAPPED_FIELD(Erd_ApplianceApiManifest, ApplianceApiManifest_t, revision),
      SWAPPED_FIELD(Erd_ApplianceApiManifest, ApplianceApiManifest_t, features),
      SWAPPED_ERD(Erd_BuildNumber, sizeof(uint32_t)),
      SWAPPED_ERD(Erd_ApplianceRunTimeInMinutes, sizeof(ApplianceRunTimeMinutes_t)),
      SWAPPED_FIELD(Erd_ApplianceApiManifest, ApplianceApiManifest_t, revision),
      SWAPPED_FIELD(Erd_ApplianceApiManifest, ApplianceApiManifest_t, features),
      SWAPPED_FIELD(Erd_TimerModuleDiagnosticsResult, TimerModuleDiagnosticsResults_t, maximumLatency),
      SWAPPED_FIELD(Erd_TimerModuleDiagnosticsResult, TimerModuleDiagnosticsResults_t, averageLatency),
      SWAPPED_ERD(Erd_SomeAnalogInput, sizeof(uint16_t)),
      SWAPPED_ERD(Erd_AnotherAnalogInput, sizeof(uint16_t)),
      SWAPPED_ERD(Erd_ServiceDiagnosticsEntityLocation, sizeof(Erd_t)),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsEntityManifest, ServiceDiagnosticsRevision3Manifest_t, entityMapStart),
      SWAPPED_ERD(Erd_ServiceDiagnosticsEntityCycleCount, sizeof(uint32_t)),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultTableStatus, ServiceDiagnosticsRevision3TableStatus_t, currentIndex),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultTableStatus, ServiceDiagnosticsRevision3TableStatus_t, count),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultTableStatus, ServiceDiagnosticsRevision3TableStatus_t, overwriteCount),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultTableStatus, ServiceDiagnosticsRevision3TableStatus_t, reserved),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultSequenceStatus, ServiceDiagnosticsRevision3SequenceStatus_t, currentIndex),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultSequenceStatus, ServiceDiagnosticsRevision3SequenceStatus_t, count),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultSequenceStatus, ServiceDiagnosticsRevision3SequenceStatus_t, overwriteCount),
      SWAPPED_FIELD(Erd_ServiceDiagnosticsFaultSequenceStatus, ServiceDiagnosticsRevision3SequenceStatus_t, reserved)
   };

static const ConstMultiMap_BinarySearchConfiguration_t endiannessSwappedMapConfiguration =
   {
      swappedFields,
      NUM_ELEMENTS(swappedFields),
      ELEMENT_SIZE(swappedFields),
      MEMBER_SIZE(DataSource_EndiannessSwappedSwappedField_t, erd),
      OFFSET_OF(DataSource_EndiannessSwappedSwappedField_t, erd),
      sizeof(DataSource_EndiannessSwappedSwappedField_t),
      0,
      false
   };

void ExternalDataSource_Init(
   ExternalDataSource_t *instance,
   I_DataSource_t *dataSource)
{
   ConstMultiMap_BinarySearch_Init(&instance->_private.endiannessSwappedMap, &endiannessSwappedMapConfiguration);
   ConstArrayMap_DirectIndex_Init(&instance->_private.internalToExternalMap, &internalToExternalMapConfiguration);
   ConstArrayMap_BinarySearch_Init(&instance->_private.externalToInternalMap, &externalToInternalMapConfiguration);
   ConstBidirectionalMap_ConstArrayMap_Init(
      &instance->_private.internalExternalMap,
      &instance->_private.externalToInternalMap.interface,
      &instance->_private.internalToExternalMap.interface);

   DataSource_EndiannessSwapped_Init(
      &instance->_private.endiannessSwappedDataSource,
      dataSource,
      &instance->_private.endiannessSwappedMap.interface);

   DataSource_Mapped_InitWithPassThrough(
      &instance->_private.mappedDataSource,
      &instance->_private.endiannessSwappedDataSource.interface,
      &instance->_private.internalExternalMap.interface);
}

I_DataSource_t *ExternalDataSource_DataSource(ExternalDataSource_t *instance)
{
   return &instance->_private.mappedDataSource.interface;
}
