/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ApplianceApiDiagnostics.h"
#include "SecondsSinceLastReset.h"
#include "ResetCount.h"
#include "SystemErds.h"
#include "utils.h"

static void UpdateSecondsSinceReset(void *context)
{
   REINTERPRET(instance, context, ApplianceApiDiagnostics_t *);
   SecondsSinceLastReset_t secondsSinceLastReset;

   DataModel_Read(instance->_private.dataModel, Erd_SecondsSinceLastReset, &secondsSinceLastReset);
   secondsSinceLastReset++;
   DataModel_Write(instance->_private.dataModel, Erd_SecondsSinceLastReset, &secondsSinceLastReset);
}

void ApplianceApiDiagnostics_Init(
   ApplianceApiDiagnostics_t *instance,
   I_DataModel_t *dataModel,
   TimerModule_t *timerModule,
   ResetReason_t resetReason)
{
   instance->_private.dataModel = dataModel;
   instance->_private.timerModule = timerModule;

   DataModel_Write(instance->_private.dataModel, Erd_ResetReason, &resetReason);

   ResetCount_t resetCount;
   DataModel_Read(dataModel, Erd_ResetCount, &resetCount);
   resetCount++;
   DataModel_Write(dataModel, Erd_ResetCount, &resetCount);

   TimerModule_StartPeriodic(
      timerModule,
      &instance->_private.secondsSinceResetTimer,
      1000,
      UpdateSecondsSinceReset,
      instance);
}
