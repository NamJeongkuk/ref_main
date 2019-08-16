/*!
 * @file
 * @brief Service Diagnostics Glue Code
 *
 * Copyright GE Appliances - GEA Confidential - All rights reserved
 */

#include "ServiceDiagnosticsPlugin.h"
#include "SystemErds.h"
#include "uassert.h"
#include "utils.h"
#include "Action_Null.h"
#include "Constants_Time.h"
#include "DataSourceErdPointerAccess.h"
#include "Erd.h"

enum
{
   ServiceDiagnosticsDataVersion_Major = 1,
   ServiceDiagnosticsDataVersion_Minor = 1
};

enum
{
   EntityMapStartErd = 0xE000,
   EntityStartLocation = 0xE100,
   EntityMapCount = 1,

   DiagnosticsApiVersion = 3,
   DiagnosticsProfileVersion = 0,

   DiagnosticEntityId = 0,

   RunTimeUpdatePeriod = MSEC_PER_MIN,
   MilliSecondsPerHour = (MSEC_PER_MIN * 60),
};

static const Erd_t snapshotErdList[] =
   {
      Erd_SomeData
   };

static const FaultId_t faultCodes[] =
   {
      ERD_TABLE(EXPAND_ERD_FAULT_CODE_TABLE_AS_FAULT_CODE)
   };

static const DataSource_ServiceDiagnosticsRevision3Entity_t serviceDiagnosticsEntity =
   {
      .manifest = Erd_ServiceDiagnosticsEntityManifest,

      .cycleCount = Erd_ServiceDiagnosticsEntityCycleCount,
      .runTime = Erd_ServiceDiagnosticsRunTimeMinutes,

      .id = DiagnosticEntityIdWasher,

      .dataVersion =
         {
            .major = ServiceDiagnosticsDataVersion_Major,
            .minor = ServiceDiagnosticsDataVersion_Minor
         },

      .faultTable =
         {
            .enabled = true,
            .includeSnapshot = true,

            .status = Erd_ServiceDiagnosticsFaultTableStatus,
            .clear = Erd_ServiceDiagnosticsFaultTableClear,

            .entryCount = Erd_ServiceDiagnosticsFaultTableEntry9 - Erd_ServiceDiagnosticsFaultTableEntry0 + 1,
            .entryStart = Erd_ServiceDiagnosticsFaultTableEntry0,
            .snapshotStart = Erd_FaultTableSnapshot0,
            .snapshotPayloadStart = Erd_FaultTableSnapshotData0
         },

      .faultSequence =
         {
            .enabled = true,
            .includeSnapshot = false,

            .status = Erd_ServiceDiagnosticsFaultSequenceStatus,
            .clear = Erd_ServiceDiagnosticsFaultSequenceClear,

            .entryCount = Erd_ServiceDiagnosticsFaultSequenceEntry07 - Erd_ServiceDiagnosticsFaultSequenceEntry00 + 1,
            .entryStart = Erd_ServiceDiagnosticsFaultSequenceEntry00,
            .snapshotStart = Erd_Invalid,
            .snapshotPayloadStart = Erd_Invalid
         },

      .cycleHistorySequence =
         {
            .enabled = false
         },

      .fault =
         {
            .start = Erd_SomeFault,
            .count = ELEMENT_COUNT(faultCodes),
            .codes = faultCodes,
         },

      .snapshot =
         {
            .erds = snapshotErdList,
            .erdCount = ELEMENT_COUNT(snapshotErdList)
         }
   };

static const DataSource_ServiceDiagnosticsRevision3Entity_t *entities[] =
   {
      &serviceDiagnosticsEntity,
   };

static const DataSource_ServiceDiagnosticsRevision3Configuration_t serviceDiagnosticsConfiguration =
   {
      .entities = entities,
      .entityCount = ELEMENT_COUNT(entities)
   };

static const PeriodicErdTransfererConfigurationEntry_t periodicErdTransferTableEntries[] =
   {
      { Erd_ApplianceRunTimeInMinutes, Erd_ApplianceRunTimeInMinutesUpdatedHourly }
   };

static const PeriodicErdTransfererConfiguration_t periodicErdTransferConfiguration =
   {
      periodicErdTransferTableEntries,
      NUM_ELEMENTS(periodicErdTransferTableEntries)
   };

static void InitializeApiVersionAndManifest(I_DataSource_t *dataSource)
{
   ServiceDiagnosticsApiVersion_t apiVersion =
      {
         .apiVersion = DiagnosticsApiVersion,
         .profile = DiagnosticsProfileVersion
      };
   DataSource_Write(dataSource, Erd_DiagnosticsApiVersion, &apiVersion);

   ServiceDiagnosticsRevision3Manifest_t manifest =
      {
         .entityMapStart = EntityMapStartErd,
         .entityCount = EntityMapCount
      };
   DataSource_Write(dataSource, Erd_ServiceDiagnosticsRevision3Manifest, &manifest);

   ServiceDiagnosticsRevision3Manifest_t test;
   DataSource_Read(dataSource, Erd_ServiceDiagnosticsRevision3Manifest, &test);

   uassert(test.entityMapStart == manifest.entityMapStart);

   Erd_t entityLocation = EntityStartLocation;
   DataSource_Write(dataSource, Erd_ServiceDiagnosticsEntityLocation, &entityLocation);
}

static void InitializeSystemTimeUpdater(
   ServiceDiagnosticsPlugin_t *instance,
   I_DataSource_t *dataSource,
   TimerModule_t *timerModule)
{
   uint32_t initialSystemTimeMin;
   DataSource_Read(dataSource, Erd_ApplianceRunTimeInMinutesUpdatedHourly, &initialSystemTimeMin);
   DataSource_Write(dataSource, Erd_ApplianceRunTimeInMinutes, &initialSystemTimeMin);

   InputOutput_DataSource_Init(
      &instance->_private.systemRuntime.inputOutput,
      dataSource,
      Erd_ApplianceRunTimeInMinutes);

   SystemRunTimeUpdater_Init(
      &instance->_private.systemRuntime.updater,
      &instance->_private.systemRuntime.inputOutput.interface,
      timerModule,
      MSEC_PER_MIN);

   PeriodicErdTransferer_Init(
      &instance->_private.periodicErdTransferer,
      dataSource,
      timerModule,
      MilliSecondsPerHour,
      &periodicErdTransferConfiguration);
}

void ServiceDiagnosticsPlugin_Init(
   ServiceDiagnosticsPlugin_t *instance,
   I_DataSource_t *dataSource,
   TimerModule_t *timerModule)
{
   InitializeApiVersionAndManifest(dataSource);

   InitializeSystemTimeUpdater(instance, dataSource, timerModule);

   DataSource_ServiceDiagnosticsRevision3_ValidateConfiguration(
      &serviceDiagnosticsConfiguration,
      dataSource);

   DataSource_ServiceDiagnosticsRevision3_Init(
      &instance->_private.serviceDiagnosticsRevision3DataSource,
      &serviceDiagnosticsConfiguration,
      dataSource);

   InputGroup_ServiceDiagnosticsRevision3FaultTableIterator_Init(
      &instance->_private.faultTableIterator,
      dataSource,
      &serviceDiagnosticsConfiguration);

   DataSourceErdPointerAccess_Write(dataSource, Erd_FaultCodeTableInputGroup, &instance->_private.faultTableIterator.interface);
}

I_DataSource_t * ServiceDiagnosticsPlugin_GetDataSource(ServiceDiagnosticsPlugin_t *instance)
{
   return &instance->_private.serviceDiagnosticsRevision3DataSource.interface;
}
