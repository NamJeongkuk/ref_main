/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideDoorPlugin.h"
#include "SystemErds.h"

static const AllFreshFoodDoorStatusConfiguration_t allFreshFoodDoorStatusConfiguration = {
   .rightFreshDoorIsOpenErd = Erd_RightSideFreshFoodDoorStatusResolved,
   .leftFreshDoorIsOpenErd = Erd_LeftSideFreshFoodDoorIsOpenResolved,
   .allFreshFoodDoorsAreClosedErd = Erd_AllFreshFoodDoorsAreClosed
};

static const SabbathDoorOverridePair_t sabbathOverrideErdPairs[] = {
   {
      .doorStatusOverrideRequestErd = Erd_LeftSideFreezerDoorStatus_SabbathOverrideRequest,
      .doorStatusOverrideValueErd = Erd_LeftSideFreezerDoorStatus_SabbathOverrideValue,
   },
   {
      .doorStatusOverrideRequestErd = Erd_RightSideFreshFoodDoorStatus_SabbathOverrideRequest,
      .doorStatusOverrideValueErd = Erd_RightSideFreshFoodDoorStatus_SabbathOverrideValue,
   },
};

static const SabbathInhibitDoorsConfiguration_t sabbathInhibitDoorsConfig = {
   .doorOverrideErdPairList = sabbathOverrideErdPairs,
   .numberOfPairs = NUM_ELEMENTS(sabbathOverrideErdPairs),
   .sabbathModeErd = Erd_SabbathModeEnable,
   .enhancedSabbathModeErd = Erd_EnhancedSabbathModeEnable,
   .sabbathGpioErd = Erd_Gpio_SABBATH
};

static const Erd_t leftSideFreezerDoorStatusOverrideRequestErds[] = {
   Erd_LeftSideFreezerDoorStatus_SabbathOverrideRequest,
   Erd_LeftSideFreezerDoorStatus_TestingOverrideRequest
};

static const Erd_t leftSideFreezerDoorStatusOverrideValueErds[] = {
   Erd_LeftSideFreezerDoorStatus,
   Erd_LeftSideFreezerDoorStatus_SabbathOverrideValue,
   Erd_LeftSideFreezerDoorStatus_TestingOverrideValue
};

static const OverrideArbiterConfiguration_t leftSideFreezerDoorStatusOverrideConfiguration = {
   .overrideRequestErds = leftSideFreezerDoorStatusOverrideRequestErds,
   .valueErds = leftSideFreezerDoorStatusOverrideValueErds,
   .resolvedErd = Erd_LeftSideFreezerDoorStatusResolved,
   .numberOfOverrideRequests = NUM_ELEMENTS(leftSideFreezerDoorStatusOverrideRequestErds)
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

void SideBySideDoorPlugin_Init(SideBySideDoorPlugin_t *instance, I_DataModel_t *dataModel)
{
   OverrideArbiter_Init(
      &instance->_private.leftSideFreezerDoorOverrideArbiter,
      DataModel_AsDataSource(dataModel),
      &leftSideFreezerDoorStatusOverrideConfiguration);

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
