/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RAKECONTROLLER_H
#define RAKECONTROLLER_H

#include "EventSubscription.h"
#include "I_DataModel.h"
#include "Timer.h"
#include "AluminumMoldIceMakerData.h"

typedef struct
{
   Erd_t rakeControlRequestErd; // bool
   Erd_t rakeMotorVoteErd; // IceMakerMotorVotedState_t
   Erd_t rakeCompletedRevolutionErd; // bool
   Erd_t timerModuleErd; // TimerModule_t *
   Erd_t rakePositionErd; // RakePosition_t
   Erd_t feelerArmPositionErd; // FeelerArmPosition_t
   Erd_t rakePositionHasNotBeenHomeErd; // bool
   Erd_t feelerArmPositionHasBeenBucketFullErd; // bool
} RakeControllerConfig_t;

typedef struct
{
   struct
   {
      EventSubscription_t rakeControlRequestSubscription;
      EventSubscription_t rakePositionSubscription;
      EventSubscription_t feelerArmPositionSubscription;
      I_DataModel_t *dataModel;
      Timer_t rakeNotHomeTestTimer;
      Timer_t feelerArmBucketFullTestTimer;
      const RakeControllerConfig_t *config;
      const AluminumMoldIceMakerHarvestData_t *harvestData;
   } _private;
} RakeController_t;

void RakeController_Init(
   RakeController_t *instance,
   I_DataModel_t *dataModel,
   const RakeControllerConfig_t *config,
   const AluminumMoldIceMakerHarvestData_t *harvestData);

#endif
