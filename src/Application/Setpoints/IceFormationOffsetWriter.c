/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "IceFormationOffsetWriter.h"
#include "SystemErds.h"

static void OnChange(void *context, const void *args)
{
   I_DataModel_t *dataModel = context;
   const bool *state = args;

   TemperatureDegFx100_t temp = 0;

   if(*state)
   {
      temp = PersonalityParametricData_Get(dataModel)->adjustedSetpointData->iceCabinetAdjustedSetpointData->iceFormationOffsetInDegFx100;
   }

   DataModel_Write(
      dataModel,
      Erd_IceCabinet_IceFormationOffsetInDegFx100,
      &temp);
}

void IceFormationOffsetWriter_Init(
   IceFormationOffsetWriter_t *instance,
   I_DataModel_t *dataModel)
{
   EventSubscription_Init(
      &instance->_private.subscription,
      dataModel,
      OnChange);

   DataModel_Subscribe(
      dataModel,
      Erd_IceCabinet_IceFormationIsActive,
      &instance->_private.subscription);
}
