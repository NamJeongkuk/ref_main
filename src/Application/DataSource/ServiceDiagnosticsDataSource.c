/*!
 * @file
 * @brief Service Diagnostics Glue Code
 *
 * Copyright GE Appliances - GEA Confidential - All rights reserved
 */

#include "ServiceDiagnosticsDataSource.h"
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
   ServiceDiagnosticsDataVersion_Minor = 0
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
   MsecPerHour = (MSEC_PER_MIN * 60)
};

static const Erd_t snapshotErdList[] = {
   Erd_SomeData
};

static const FaultId_t faultCodes[] = {
   ERD_TABLE(EXPAND_AS_FAULT_ID)
};

// clang-format off
static const DataSource_ServiceDiagnosticsRevision3Entity_t serviceDiagnosticsEntity =
   {
      .manifest = Erd_ServiceDiagnosticsEntityManifest,

      .cycleCount = Erd_ServiceDiagnosticsEntityCycleCount,
      .runTime = Erd_ServiceDiagnosticsRunTimeInMinutes,

      .id = DiagnosticEntityId,

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
            .snapshotStart = Erd_ServiceDiagnosticsFaultTableSnapshot0,
            .snapshotPayloadStart = Erd_ServiceDiagnosticsFaultTableSnapshotData0
         },

      .faultSequence =
         {
            .enabled = true,
            .includeSnapshot = false,

            .status = Erd_ServiceDiagnosticsFaultSequenceStatus,
            .clear = Erd_ServiceDiagnosticsFaultSequenceClear,

            .entryCount = Erd_ServiceDiagnosticsFaultSequenceEntry7 - Erd_ServiceDiagnosticsFaultSequenceEntry0 + 1,
            .entryStart = Erd_ServiceDiagnosticsFaultSequenceEntry0,
            .snapshotStart = Erd_Invalid,
            .snapshotPayloadStart = Erd_Invalid
         },

      .cycleHistorySequence =
         {
            .enabled = true,

            .status = Erd_ServiceDiagnosticsCycleHistorySequenceStatus,
            .clear = Erd_ServiceDiagnosticsCycleHistorySequenceClear,

            .entryCount = Erd_DiagnosticsCycleHistoryRecord9 - Erd_DiagnosticsCycleHistoryRecord0 + 1,
            .entryStart = Erd_DiagnosticsCycleHistoryRecord0,
            .addEntry = Erd_ServiceDiagnosticsCycleHistoryAddEntry
         },

      .fault =
         {
            .start = Erd_DoorBoardCommunicationFault,
            .count = ELEMENT_COUNT(faultCodes),
            .codes = faultCodes,
         },

      .snapshot =
         {
            .erds = snapshotErdList,
            .erdCount = ELEMENT_COUNT(snapshotErdList)
         }
   };
// clang-format on

static const DataSource_ServiceDiagnosticsRevision3Entity_t *entities[] = {
   &serviceDiagnosticsEntity,
};

static const DataSource_ServiceDiagnosticsRevision3Configuration_t serviceDiagnosticsConfiguration = {
   .entities = entities,
   .entityCount = ELEMENT_COUNT(entities)
};

static const PeriodicErdTransfererConfigurationEntry_t periodicErdTransferTableEntries[] = {
   { Erd_ServiceDiagnosticsRunTimeInMinutes, Erd_ApplianceRunTimeInMinutesUpdatedHourly }
};

static const PeriodicErdTransfererConfiguration_t periodicErdTransferConfiguration = {
   periodicErdTransferTableEntries,
   NUM_ELEMENTS(periodicErdTransferTableEntries)
};

static void InitializeApiVersionAndManifest(I_DataSource_t *dataSource)
{
   ServiceDiagnosticsApiVersion_t apiVersion = {
      .apiVersion = DiagnosticsApiVersion,
      .profile = DiagnosticsProfileVersion
   };
   DataSource_Write(dataSource, Erd_ServiceDiagnosticsApiVersion, &apiVersion);

   ServiceDiagnosticsRevision3Manifest_t manifest = {
      .entityMapStart = EntityMapStartErd,
      .entityCount = EntityMapCount
   };
   DataSource_Write(dataSource, Erd_ServiceDiagnosticsRevision3Manifest, &manifest);

   Erd_t entityLocation = EntityStartLocation;
   DataSource_Write(dataSource, Erd_ServiceDiagnosticsEntityLocation, &entityLocation);
}

static void InitializeSystemTimeUpdater(
   ServiceDiagnosticsDataSource_t *instance,
   I_DataSource_t *dataSource,
   TimerModule_t *timerModule)
{
   uint32_t initialSystemTimeMin;
   DataSource_Read(dataSource, Erd_ApplianceRunTimeInMinutesUpdatedHourly, &initialSystemTimeMin);
   DataSource_Write(dataSource, Erd_ServiceDiagnosticsRunTimeInMinutes, &initialSystemTimeMin);

   InputOutput_DataSource_Init(
      &instance->_private.systemRuntime.inputOutput,
      dataSource,
      Erd_ServiceDiagnosticsRunTimeInMinutes);

   SystemRunTimeUpdater_Init(
      &instance->_private.systemRuntime.updater,
      &instance->_private.systemRuntime.inputOutput.interface,
      timerModule,
      MSEC_PER_MIN);

   PeriodicErdTransferer_Init(
      &instance->_private.periodicErdTransferer,
      dataSource,
      timerModule,
      MsecPerHour,
      &periodicErdTransferConfiguration);
}

void ServiceDiagnosticsDataSource_Init(
   ServiceDiagnosticsDataSource_t *instance,
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

I_DataSource_t *ServiceDiagnosticsDataSource_DataSource(ServiceDiagnosticsDataSource_t *instance)
{
   return &instance->_private.serviceDiagnosticsRevision3DataSource.interface;
}
