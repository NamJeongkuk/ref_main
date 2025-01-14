/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SYSTEMDATA_H
#define SYSTEMDATA_H

#include "RamDataSource.h"
#include "NonVolatileDataSource.h"
#include "BspDataSource.h"
#include "UnmappedBspDataSource.h"
#include "ApplianceApiDataSource.h"
#include "DataSource_Composite.h"
#include "DataModel_DataSource.h"
#include "DataSource_Mapped.h"
#include "ExternalDataSource.h"
#include "DataModel.h"
#include "ServiceDiagnosticsDataSource.h"
#include "ConvertedDataSource.h"
#include "SystemErds.h"
#include "I_Interrupt.h"
#include "Timer.h"

typedef struct
{
   struct
   {
      struct
      {
         union
         {
            I_DataSource_t internal;
            DataSource_Composite_t composite;
         };

         union
         {
            I_DataSource_t external;
            ExternalDataSource_t externalDataSource;
         };

         RamDataSource_t ram;
         DataSource_CompositeComponent_t ramComponent;

         NonVolatileDataSource_t nv;
         DataSource_CompositeComponent_t nvComponent;

         BspDataSource_t bsp;
         DataSource_CompositeComponent_t bspComponent;

         UnmappedBspDataSource_t unmappedBsp;
         DataSource_CompositeComponent_t unmappedBspComponent;

         ApplianceApiDataSource_t applianceApi;
         DataSource_CompositeComponent_t applianceApiComponent;

         ServiceDiagnosticsDataSource_t serviceDiagnostics;
         DataSource_CompositeComponent_t serviceDiagnosticsComponent;

         ConvertedDataSource_t converted;
         DataSource_CompositeComponent_t convertedComponent;
      } dataSource;

      DataModel_t dataModel;
   } _private;
} SystemData_t;

/*!
 * @param instance
 * @param timerModule
 * @param async
 * @param crcCalculator
 * @param systemActionForStartup
 * @param resetAction
 */
void SystemData_Init(
   SystemData_t *instance,
   TimerModule_t *timerModule,
   I_AsyncDataSource_t *async,
   I_Crc16Calculator_t *crcCalculator,
   I_Action_t *systemActionForStartup,
   I_Action_t *resetAction);

/*!
 * @param instance
 * @return
 */
I_DataSource_t *SystemData_InternalDataSource(
   SystemData_t *instance);

/*!
 * @param instance
 * @return
 */
I_DataSource_t *SystemData_ExternalDataSource(
   SystemData_t *instance);

/*!
 * @param instance
 * @return
 */
I_DataModel_t *SystemData_DataModel(
   SystemData_t *instance);

/*!
 * @param instance
 * @return I_DataSource_t*
 */
I_DataSource_t *SystemData_BspDataSource(SystemData_t *instance);

/*!
 * @param instance
 * @param dataModel
 * @param timerModule
 * @param interrupt
 */
void SystemData_AddBspDataSource(
   SystemData_t *instance,
   I_DataModel_t *dataModel,
   TimerModule_t *timerModule,
   I_Interrupt_t *interrupt);

#endif
