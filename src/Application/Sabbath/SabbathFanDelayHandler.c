/*!
 * @file
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SabbathFanDelayHandler.h"
#include "CompressorState.h"
#include "FanSpeed.h"
#include "Vote.h"

static void HandleSabbathFanDelay(void *context, const void *args)
{
   SabbathFanDelayHandler_t *instance = context;
   IGNORE(args);

   CompressorState_t currentState;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorStateErd,
      &currentState);

   if(currentState == CompressorState_SabbathDelay)
   {
      FanVotedSpeed_t fanSabbathVoteOff = { .speed = FanSpeed_Off, .care = Vote_Care };
      ErdList_WriteAll(
         instance->_private.dataModel,
         &instance->_private.config->sabbathFanVoteErdList,
         &fanSabbathVoteOff);
   }
   else
   {
      FanVotedSpeed_t fanSabbathVoteDontCare = { .speed = FanSpeed_Off, .care = Vote_DontCare };
      ErdList_WriteAll(
         instance->_private.dataModel,
         &instance->_private.config->sabbathFanVoteErdList,
         &fanSabbathVoteDontCare);
   }
}

void SabbathFanDelayHandler_Init(
   SabbathFanDelayHandler_t *instance,
   I_DataModel_t *dataModel,
   const SabbathFanDelayHandlerConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   HandleSabbathFanDelay(instance, NULL);

   EventSubscription_Init(
      &instance->_private.compressorStateSubscription,
      instance,
      HandleSabbathFanDelay);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->compressorStateErd,
      &instance->_private.compressorStateSubscription);
}
