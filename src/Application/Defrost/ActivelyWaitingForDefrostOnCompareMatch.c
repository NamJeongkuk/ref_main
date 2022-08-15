/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ActivelyWaitingForDefrostOnCompareMatch.h"
#include "DefrostHsmState.h"
#include "SystemErds.h"
#include "Constants_Binary.h"

static struct
{
   EventSubscription_t subscription;
} instance;

static void UpdateActivelyWaitingForNextDefrostBasedOnDefrostHsmState(void *context, const void *args)
{
   I_DataModel_t *dataModel = context;
   const DefrostHsmState_t *state = args;

   if(*state == DefrostHsmState_PostDwell || *state == DefrostHsmState_Idle)
   {
      DataModel_Write(
         dataModel,
         Erd_ActivelyWaitingForNextDefrost,
         set);
   }
   else
   {
      DataModel_Write(
         dataModel,
         Erd_ActivelyWaitingForNextDefrost,
         clear);
   }
}

void ActivelyWaitingForDefrostOnCompareMatch(I_DataModel_t *dataModel)
{
   DefrostHsmState_t state;
   DataModel_Read(
      dataModel,
      Erd_DefrostHsmState,
      &state);

   UpdateActivelyWaitingForNextDefrostBasedOnDefrostHsmState(dataModel, &state);

   EventSubscription_Init(
      &instance.subscription,
      dataModel,
      UpdateActivelyWaitingForNextDefrostBasedOnDefrostHsmState);
   DataModel_Subscribe(
      dataModel,
      Erd_DefrostHsmState,
      &instance.subscription);

   DataModel_Write(
      dataModel,
      Erd_ActivelyWaitingForDefrostOnCompareMatchReady,
      set);
}
