/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SingleDoorFreshFoodDoorPlugin.h"
#include "SystemErds.h"

static const AllFreshFoodDoorStatusConfiguration_t allFreshFoodDoorStatusConfiguration = {
   .rightFreshDoorStatusErd = Erd_RightSideFreshFoodDoorStatusResolved,
   .leftFreshDoorStatusErd = Erd_LeftSideFreshFoodDoorStatusResolved,
   .allFreshFoodDoorsAreClosedErd = Erd_AllFreshFoodDoorsAreClosed
};

static const SabbathDoorOverridePair_t sabbathOverrideErdPairs[] = {
   {
      .doorStatusOverrideRequestErd = Erd_RightSideFreshFoodDoorStatus_SabbathOverrideRequest,
      .doorStatusOverrideValueErd = Erd_RightSideFreshFoodDoorStatus_SabbathOverrideValue,
   }
};

static const SabbathInhibitDoorsConfiguration_t sabbathInhibitDoorsConfig = {
   .doorOverrideErdPairList = sabbathOverrideErdPairs,
   .numberOfPairs = NUM_ELEMENTS(sabbathOverrideErdPairs),
   .sabbathModeErd = Erd_SabbathModeEnable,
   .enhancedSabbathModeErd = Erd_EnhancedSabbathModeEnable,
   .sabbathGpioErd = Erd_Gpio_SABBATH
};

static const Erd_t rightSideFreshFoodDoorStatusOverrideRequestErds[] = {
   Erd_RightSideFreshFoodDoorStatus_SabbathOverrideRequest,
   Erd_RightSideFreshFoodDoorStatus_TestingOverrideRequest
};

static const Erd_t rightSideFreshFoodDoorOverrideValueErds[] = {
   Erd_RightSideFreshFoodDoorStatus,
   Erd_RightSideFreshFoodDoorStatus_SabbathOverrideValue,
   Erd_RightSideFreshFoodDoorStatus_TestingOverrideValue
};

static const OverrideArbiterConfiguration_t rightSideFreshFoodDoorStatusOverrideConfiguration = {
   .overrideRequestErds = rightSideFreshFoodDoorStatusOverrideRequestErds,
   .valueErds = rightSideFreshFoodDoorOverrideValueErds,
   .resolvedErd = Erd_RightSideFreshFoodDoorStatusResolved,
   .numberOfOverrideRequests = NUM_ELEMENTS(rightSideFreshFoodDoorStatusOverrideRequestErds)
};

void SingleDoorFreshFoodDoorPlugin_Init(
   SingleDoorFreshFoodDoorPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   OverrideArbiter_Init(
      &instance->_private.rightSideFreshFoodDoorOverrideArbiter,
      DataModel_AsDataSource(dataModel),
      &rightSideFreshFoodDoorStatusOverrideConfiguration);

   SabbathInhibitDoors_Init(
      &instance->_private.sabbathInhibitDoors,
      dataModel,
      &sabbathInhibitDoorsConfig);

   AllFreshFoodDoorStatus_Init(
      &instance->_private.allFreshFoodDoorStatus,
      dataModel,
      &allFreshFoodDoorStatusConfiguration);
}
