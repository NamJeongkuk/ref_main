/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SabbathDelayHandler.h"
#include "CompressorVotedSpeed.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Time.h"
#include "PersonalityParametricData.h"
#include "StackAllocator.h"

typedef SabbathDelayHandler_t Instance_t;

static bool CompressorIsSwitchingToOn(Instance_t *instance, CompressorSpeed_t newSpeed)
{
   return ((instance->_private.previousCompressorSpeed == CompressorSpeed_Off) && (newSpeed != CompressorSpeed_Off));
}

static void WriteToLoadErd(void *context, void *allocation)
{
   Instance_t *instance = context;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->loadResolvedVote,
      allocation);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->loadResolvedVoteWithSabbathDelay,
      allocation);
}

static void PassThroughLoadVote(void *context)
{
   Instance_t *instance = context;

   uint8_t erdSize = DataModel_SizeOf(
      instance->_private.dataModel,
      instance->_private.config->loadResolvedVote);

   StackAllocator_Allocate(erdSize, WriteToLoadErd, instance);
}

static bool SabbathModeIsActive(Instance_t *instance)
{
   bool sabbathModeActive;
   DataModel_Read(
      instance->_private.dataModel,
      Erd_SabbathModeEnable,
      &sabbathModeActive);

   bool enhancedSabbathModeActive;
   DataModel_Read(
      instance->_private.dataModel,
      Erd_EnhancedSabbathModeEnable,
      &enhancedSabbathModeActive);

   return (sabbathModeActive || enhancedSabbathModeActive);
}

static bool SabbathDelayIsNotActive(Instance_t *instance)
{
   return !TimerModule_IsRunning(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.timer);
}

static void DataModelChanged(void *context, const void *_args)
{
   Instance_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == instance->_private.config->compressorResolvedVote)
   {
      const CompressorVotedSpeed_t *newVote = args->data;

      if(CompressorIsSwitchingToOn(instance, newVote->speed) && SabbathModeIsActive(instance))
      {
         TimerModule_StartOneShot(
            DataModelErdPointerAccess_GetTimerModule(
               instance->_private.dataModel,
               Erd_TimerModule),
            &instance->_private.timer,
            PersonalityParametricData_Get(
               instance->_private.dataModel)
                  ->compressorData->compressorTimes.sabbathDelayTimeInSeconds *
               MSEC_PER_SEC,
            PassThroughLoadVote,
            instance);
      }
      instance->_private.previousCompressorSpeed = newVote->speed;
   }
   if(args->erd == instance->_private.config->loadResolvedVote)
   {
      if(SabbathDelayIsNotActive(instance))
      {
         PassThroughLoadVote(instance);
      }
   }
}

void SabbathDelayHandler_Init(
   SabbathDelayHandler_t *instance,
   I_DataModel_t *dataModel,
   const SabbathDelayHandlerConfiguration_t *config)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;

   CompressorVotedSpeed_t vote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorResolvedVote,
      &vote);
   instance->_private.previousCompressorSpeed = vote.speed;

   PassThroughLoadVote(instance);

   EventSubscription_Init(
      &instance->_private.dataModelSubcription,
      instance,
      DataModelChanged);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.dataModelSubcription);
}
