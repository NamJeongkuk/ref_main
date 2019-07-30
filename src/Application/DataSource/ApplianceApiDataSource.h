/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef APPLIANCEAPIDATASOURCE_H
#define APPLIANCEAPIDATASOURCE_H

#include "DataSource_ApplianceApiRevision1.h"

typedef struct
{
   struct
   {
      DataSource_ApplianceApiRevision1_t dataSource;
   } _private;
} ApplianceApiDataSource_t;

/*!
 * @param instance
 * @param dataSource
 * @param timerModule
 * @param resetAction
 * @param isPyramid
 */
void ApplianceApiDataSource_Init(
   ApplianceApiDataSource_t *instance,
   I_DataSource_t *dataSource,
   TimerModule_t *timerModule,
   I_Action_t *resetAction,
   I_Crc16Calculator_t *crcCalculator);

/*!
 * @param instance
 * @return
 */
I_DataSource_t * ApplianceApiDataSource_DataSource(
  ApplianceApiDataSource_t *instance);

#endif
