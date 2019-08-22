/*!
 * @file
 * @brief d
 *
 * Copyright GE Appliances - GEA Confidential - All rights reserved
 */

#ifndef SERVICEDIAGNOSTICSDATASOURCE_H
#define SERVICEDIAGNOSTICSDATASOURCE_H

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
} ServiceDiagnosticsDataSource_t;

/*!
 * Initializes the service diagnostic data source.
 * @param instance
 * @param dataSource The system dataSource that contains all other data sources
 * @param timerModule
 */
void ServiceDiagnosticsDataSource_Init(
   ServiceDiagnosticsDataSource_t *instance,
   I_DataSource_t *dataSource,
   TimerModule_t *timerModule);

/*!
 * @param instance
 * @return
 */
I_DataSource_t * ServiceDiagnosticsDataSource_DataSource(ServiceDiagnosticsDataSource_t *instance);

#endif
