/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CompressorIsOn.h"
#include "I_DataModel.h"
#include "SystemErds.h"
#include "CompressorState.h"
#include "Constants_Binary.h"

static struct
{
   EventSubscription_t subscription;
} instance;

static void UpdateCompressorIsOnBasedOnCompressorState(void *context, const void *args)
{
   I_DataModel_t *dataModel = context;
   const CompressorState_t *state = args;

   if(*state == CompressorState_OnAndReadyToChange ||
      *state == CompressorState_MinimumOnTime ||
      *state == CompressorState_VariableSpeedMinimumRunTime)
   {
      DataModel_Write(
         dataModel,
         Erd_CompressorIsOn,
         on);
   }
   else
   {
      DataModel_Write(
         dataModel,
         Erd_CompressorIsOn,
         off);
   }
}

void CompressorIsOn_Init(I_DataModel_t *dataModel)
{
   CompressorState_t state;
   DataModel_Read(
      dataModel,
      Erd_CompressorState,
      &state);

   UpdateCompressorIsOnBasedOnCompressorState(dataModel, &state);

   EventSubscription_Init(
      &instance.subscription,
      dataModel,
      UpdateCompressorIsOnBasedOnCompressorState);
   DataModel_Subscribe(
      dataModel,
      Erd_CompressorState,
      &instance.subscription);
}
