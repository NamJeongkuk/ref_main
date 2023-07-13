/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SabbathPlugin.h"
#include "Constants_Binary.h"
#include "SabbathReadyToDefrost.h"
#include "SystemErds.h"

static struct
{
   SabbathReadyToDefrost_t sabbathReadyToDefrost;
} instance;

static const SabbathReadyToDefrostConfig_t sabbathReadyToDefrostConfig = {
   .timerModuleErd = Erd_TimerModule,
   .waitingToDefrostErd = Erd_WaitingToDefrost,
   .sabbathIsReadyToDefrostErd = Erd_SabbathIsReadyToDefrost,
   .sabbathTimeBetweenDefrostsInMinutesErd = Erd_SabbathTimeBetweenDefrostsInMinutes,
   .sabbathWaitingForDefrostTimeInMinutesErd = Erd_SabbathWaitingForDefrostTimeInMinutes
};

void SabbathPlugin_Init(I_DataModel_t *dataModel)
{
   SabbathReadyToDefrost_Init(
      &instance.sabbathReadyToDefrost,
      dataModel,
      &sabbathReadyToDefrostConfig,
      PersonalityParametricData_Get(dataModel)->sabbathData);

   DataModel_Write(
      dataModel,
      Erd_SabbathPluginReady,
      set);
}
