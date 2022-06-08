/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ActivelyWaitingForDefrostOnCompareMatch.h"
#include "ErdWriterOnCompareMatch.h"
#include "DefrostHsmState.h"
#include "SystemErds.h"
#include "Constants_Binary.h"

static struct
{
   EventSubscription_t subscription;
   I_DataModel_t *dataModel;
} instance;

static void UpdateActivelyWaitingForNextDefrostBasedOnDefrostHsmState(void *context, const void *args)
{
   IGNORE(context);
   REINTERPRET(state, args, const DefrostHsmState_t *);

   if(*state == DefrostHsmState_PostDwell || *state == DefrostHsmState_Idle)
   {
      DataModel_Write(
         instance.dataModel,
         Erd_ActivelyWaitingForNextDefrost,
         set);
   }
   else
   {
      DataModel_Write(
         instance.dataModel,
         Erd_ActivelyWaitingForNextDefrost,
         clear);
   }
}

void ActivelyWaitingForDefrostOnCompareMatch(I_DataModel_t *dataModel)
{
   instance.dataModel = dataModel;

   DefrostHsmState_t state;
   DataModel_Read(
      instance.dataModel,
      Erd_DefrostHsmState,
      &state);

   UpdateActivelyWaitingForNextDefrostBasedOnDefrostHsmState(NULL, &state);

   EventSubscription_Init(
      &instance.subscription,
      NULL,
      UpdateActivelyWaitingForNextDefrostBasedOnDefrostHsmState);
   DataModel_Subscribe(
      dataModel,
      Erd_DefrostHsmState,
      &instance.subscription);
}
