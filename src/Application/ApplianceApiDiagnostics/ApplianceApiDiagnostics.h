/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef APPLIANCEAPIDIAGNOSTICS_H
#define APPLIANCEAPIDIAGNOSTICS_H

#include "I_DataModel.h"
#include "Timer.h"
#include "ResetReason.h"

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      TimerModule_t *timerModule;
      Timer_t secondsSinceResetTimer;
   } _private;
} ApplianceApiDiagnostics_t;

/*!
 *@param instance
 *@param dataModel
 *@param timerModule
 *@param reason
*/
void ApplianceApiDiagnostics_Init(
   ApplianceApiDiagnostics_t *instance,
   I_DataModel_t *dataModel,
   TimerModule_t *timerModule,
   ResetReason_t reason);

#endif
