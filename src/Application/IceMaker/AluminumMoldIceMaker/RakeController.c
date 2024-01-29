/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "RakeController.h"
#include "IceMakerMotorVotedState.h"
#include "Constants_Time.h"
#include "DataModelErdPointerAccess.h"
#include "FeelerArmPosition.h"
#include "RakePosition.h"
#include "PersonalityParametricData.h"

static void VoteForRakeMotor(
   RakeController_t *instance,
   MotorState_t state,
   Vote_t care)
{
   IceMakerMotorVotedState_t rakeMotorVote = {
      .state = state,
      .care = care
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->rakeMotorVoteErd,
      &rakeMotorVote);
}

static void RakeCompletedRevolutionIsSetTo(
   RakeController_t *instance,
   bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->rakeCompletedRevolutionErd,
      &state);
}

static void RakePositionHasNotBeenHomeIsSetTo(
   RakeController_t *instance,
   bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->rakePositionHasNotBeenHomeErd,
      &state);
}

static void FeelerArmPositionHasBeenBucketFullIsSetTo(
   RakeController_t *instance,
   bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->feelerArmPositionHasBeenBucketFullErd,
      &state);
}

static void RakeNotHomeTestTimerExpired(void *context)
{
   RakeController_t *instance = context;
   RakePositionHasNotBeenHomeIsSetTo(instance, true);
}

static void StartRakeIsNotHomeTimer(RakeController_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.rakeNotHomeTestTimer,
      instance->_private.harvestData->rakeNotHomeTestTimeInSeconds * MSEC_PER_SEC,
      RakeNotHomeTestTimerExpired,
      instance);
}

static bool RakePositionHasNotBeenHome(RakeController_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rakePositionHasNotBeenHomeErd,
      &state);

   return state;
}

static bool FeelerArmPositionHasBeenBucketFull(RakeController_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->feelerArmPositionHasBeenBucketFullErd,
      &state);

   return state;
}

static bool RakePositionIsHome(RakeController_t *instance)
{
   RakePosition_t rakePosition;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rakePositionErd,
      &rakePosition);

   return (rakePosition == RakePosition_Home);
}

static bool RakeRevolutionHasCompleted(RakeController_t *instance)
{
   return (RakePositionHasNotBeenHome(instance) &&
      FeelerArmPositionHasBeenBucketFull(instance) &&
      RakePositionIsHome(instance));
}

static void StopRakeControlWhenRakeRevolutionHasCompleted(RakeController_t *instance)
{
   if(RakeRevolutionHasCompleted(instance))
   {
      VoteForRakeMotor(instance, MotorState_Off, Vote_Care);

      DataModel_Unsubscribe(
         instance->_private.dataModel,
         instance->_private.config->rakePositionErd,
         &instance->_private.rakePositionSubscription);

      RakeCompletedRevolutionIsSetTo(instance, true);
   }
}

static void FeelerArmBucketFullTestTimerExpired(void *context)
{
   RakeController_t *instance = context;
   FeelerArmPositionHasBeenBucketFullIsSetTo(instance, true);

   DataModel_Unsubscribe(
      instance->_private.dataModel,
      instance->_private.config->feelerArmPositionErd,
      &instance->_private.feelerArmPositionSubscription);

   StopRakeControlWhenRakeRevolutionHasCompleted(instance);
}

static void StartBucketIsFullTestTimer(RakeController_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.feelerArmBucketFullTestTimer,
      instance->_private.harvestData->feelerArmTestTimeInSeconds * MSEC_PER_SEC,
      FeelerArmBucketFullTestTimerExpired,
      instance);
}

static bool BucketIsFull(RakeController_t *instance)
{
   FeelerArmPosition_t bucketState;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->feelerArmPositionErd,
      &bucketState);

   return (bucketState == FeelerArmPosition_BucketFull);
}

static void RakeControlRequestUpdated(void *context, const void *args)
{
   RakeController_t *instance = context;
   const bool *rakeControlRequestIsSet = args;

   if(*rakeControlRequestIsSet)
   {
      VoteForRakeMotor(instance, MotorState_On, Vote_Care);

      DataModel_Subscribe(
         instance->_private.dataModel,
         instance->_private.config->rakePositionErd,
         &instance->_private.rakePositionSubscription);
      DataModel_Subscribe(
         instance->_private.dataModel,
         instance->_private.config->feelerArmPositionErd,
         &instance->_private.feelerArmPositionSubscription);

      if(!RakePositionIsHome(instance))
      {
         StartRakeIsNotHomeTimer(instance);
      }

      if(BucketIsFull(instance))
      {
         StartBucketIsFullTestTimer(instance);
      }
   }
   else
   {
      VoteForRakeMotor(instance, MotorState_Off, Vote_Care);

      DataModel_Unsubscribe(
         instance->_private.dataModel,
         instance->_private.config->rakePositionErd,
         &instance->_private.rakePositionSubscription);
      DataModel_Unsubscribe(
         instance->_private.dataModel,
         instance->_private.config->feelerArmPositionErd,
         &instance->_private.feelerArmPositionSubscription);

      TimerModule_Stop(
         DataModelErdPointerAccess_GetTimerModule(
            instance->_private.dataModel,
            instance->_private.config->timerModuleErd),
         &instance->_private.rakeNotHomeTestTimer);
      TimerModule_Stop(
         DataModelErdPointerAccess_GetTimerModule(
            instance->_private.dataModel,
            instance->_private.config->timerModuleErd),
         &instance->_private.feelerArmBucketFullTestTimer);

      RakeCompletedRevolutionIsSetTo(instance, false);
      RakePositionHasNotBeenHomeIsSetTo(instance, false);
      FeelerArmPositionHasBeenBucketFullIsSetTo(instance, false);
   }
}

static void RakePositionUpdated(void *context, const void *args)
{
   RakeController_t *instance = context;
   const RakePosition_t *rakePosition = args;

   if(*rakePosition == RakePosition_NotHome)
   {
      StartRakeIsNotHomeTimer(instance);
   }
   else
   {
      TimerModule_Stop(
         DataModelErdPointerAccess_GetTimerModule(
            instance->_private.dataModel,
            instance->_private.config->timerModuleErd),
         &instance->_private.rakeNotHomeTestTimer);

      StopRakeControlWhenRakeRevolutionHasCompleted(instance);
   }
}

static void FeelerArmPositionUpdated(void *context, const void *args)
{
   RakeController_t *instance = context;
   const FeelerArmPosition_t *feelerArmPosition = args;

   if(*feelerArmPosition == FeelerArmPosition_BucketFull)
   {
      StartBucketIsFullTestTimer(instance);
   }
   else
   {
      TimerModule_Stop(
         DataModelErdPointerAccess_GetTimerModule(
            instance->_private.dataModel,
            instance->_private.config->timerModuleErd),
         &instance->_private.feelerArmBucketFullTestTimer);
   }
}

void RakeController_Init(
   RakeController_t *instance,
   I_DataModel_t *dataModel,
   const RakeControllerConfig_t *config,
   const AluminumMoldIceMakerHarvestData_t *harvestData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.harvestData = harvestData;

   EventSubscription_Init(
      &instance->_private.rakeControlRequestSubscription,
      instance,
      RakeControlRequestUpdated);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->rakeControlRequestErd,
      &instance->_private.rakeControlRequestSubscription);

   EventSubscription_Init(
      &instance->_private.rakePositionSubscription,
      instance,
      RakePositionUpdated);

   EventSubscription_Init(
      &instance->_private.feelerArmPositionSubscription,
      instance,
      FeelerArmPositionUpdated);
}
