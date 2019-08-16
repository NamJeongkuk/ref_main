/*!
 * @file
 * @brief Creates the DataSource_ServiceDiagnosticsRevision3_t
 * Defines the configuration for it.
 * Provides other ServiceDiagnosticsRevision3 support
 *
 * Copyright GE Appliances - GEA Confidential - All rights reserved
 */

#ifndef SERVICEDIAGNOSTICSPLUGIN_H
#define SERVICEDIAGNOSTICSPLUGIN_H

#include "I_DataSource.h"
#include "Timer.h"
#include "InputOutput_DataSource.h"
#include "DataSource_ServiceDiagnosticsRevision3.h"
#include "InputGroup_ServiceDiagnosticsRevision3FaultTableIterator.h"
#include "SystemRunTimeUpdater.h"
#include "PeriodicErdTransferer.h"

typedef struct
{
   struct
   {
      DataSource_ServiceDiagnosticsRevision3_t serviceDiagnosticsRevision3DataSource;
      InputGroup_ServiceDiagnosticsRevision3FaultTableIterator_t faultTableIterator;
      PeriodicErdTransferer_t periodicErdTransferer;

      struct
      {
         InputOutput_DataSource_t inputOutput;
         SystemRunTimeUpdater_t updater;
      } systemRuntime;
   } _private;
} ServiceDiagnosticsPlugin_t;

/*!
 * Initializes the service diagnostic plugin.
 * @param instance
 * @param dataSource The system dataSource that contains all other dataSources except the one that is being created in this plugin
 * @param timerModule
 */
void ServiceDiagnosticsPlugin_Init(
   ServiceDiagnosticsPlugin_t *instance,
   I_DataSource_t *dataSource,
   TimerModule_t *timerModule);

/*!
 *Provides th service diagnostics data source
 * @param instance
 * @return The DataSource Created by this plugin
 */
I_DataSource_t * ServiceDiagnosticsPlugin_GetDataSource(ServiceDiagnosticsPlugin_t *instance);

#endif
