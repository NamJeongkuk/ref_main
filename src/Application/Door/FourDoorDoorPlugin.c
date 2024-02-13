/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FourDoorDoorPlugin.h"
#include "SystemErds.h"

static const AllFreshFoodDoorStatusConfiguration_t allFreshFoodDoorStatusConfiguration = {
   .rightFreshDoorStatusErd = Erd_RightSideFreshFoodDoorStatusResolved,
   .leftFreshDoorStatusErd = Erd_LeftSideFreshFoodDoorStatusResolved,
   .allFreshFoodDoorsAreClosedErd = Erd_AllFreshFoodDoorsAreClosed
};

static const SabbathDoorOverridePair_t sabbathOverrideErdPairs[] = {
   {
      .doorStatusOverrideRequestErd = Erd_LeftSideFreshFoodDoorStatus_SabbathOverrideRequest,
      .doorStatusOverrideValueErd = Erd_LeftSideFreshFoodDoorStatus_SabbathOverrideValue,
   },
   {
      .doorStatusOverrideRequestErd = Erd_RightSideFreshFoodDoorStatus_SabbathOverrideRequest,
      .doorStatusOverrideValueErd = Erd_RightSideFreshFoodDoorStatus_SabbathOverrideValue,
   },
   {
      .doorStatusOverrideRequestErd = Erd_ConvertibleCompartmentStatus_SabbathOverrideRequest,
      .doorStatusOverrideValueErd = Erd_ConvertibleCompartmentStatus_SabbathOverrideValue,
   },
   {
      .doorStatusOverrideRequestErd = Erd_BottomFreezerDrawerStatus_SabbathOverrideRequest,
      .doorStatusOverrideValueErd = Erd_BottomFreezerDrawerStatus_SabbathOverrideValue,
   },
};

static const SabbathInhibitDoorsConfiguration_t sabbathInhibitDoorsConfig = {
   .doorOverrideErdPairList = sabbathOverrideErdPairs,
   .numberOfPairs = NUM_ELEMENTS(sabbathOverrideErdPairs),
   .sabbathModeErd = Erd_SabbathModeEnable,
   .enhancedSabbathModeErd = Erd_EnhancedSabbathModeEnable,
   .sabbathGpioErd = Erd_Gpio_SABBATH
};

static const Erd_t leftSideFreshFoodDoorStatusOverrideRequestErds[] = {
   Erd_LeftSideFreshFoodDoorStatus_SabbathOverrideRequest,
   Erd_LeftSideFreshFoodDoorStatus_TestingOverrideRequest
};

static const Erd_t leftSideFreshFoodDoorStatusOverrideValueErds[] = {
   Erd_LeftSideFreshFoodDoorStatus,
   Erd_LeftSideFreshFoodDoorStatus_SabbathOverrideValue,
   Erd_LeftSideFreshFoodDoorStatus_TestingOverrideValue
};

static const OverrideArbiterConfiguration_t leftSideFreshFoodDoorStatusOverrideConfiguration = {
   .overrideRequestErds = leftSideFreshFoodDoorStatusOverrideRequestErds,
   .valueErds = leftSideFreshFoodDoorStatusOverrideValueErds,
   .resolvedErd = Erd_LeftSideFreshFoodDoorStatusResolved,
   .numberOfOverrideRequests = NUM_ELEMENTS(leftSideFreshFoodDoorStatusOverrideRequestErds)
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

static const Erd_t convertibleCompartmentDoorStatusOverrideRequestErds[] = {
   Erd_ConvertibleCompartmentStatus_SabbathOverrideRequest,
   Erd_ConvertibleCompartmentStatus_TestingOverrideRequest
};

static const Erd_t convertibleCompartmentDoorStatusOverrideValueErds[] = {
   Erd_ConvertibleCompartmentDoorStatus,
   Erd_ConvertibleCompartmentStatus_SabbathOverrideValue,
   Erd_ConvertibleCompartmentStatus_TestingOverrideValue
};

static const OverrideArbiterConfiguration_t convertibleCompartmentDoorStatusOverrideConfiguration = {
   .overrideRequestErds = convertibleCompartmentDoorStatusOverrideRequestErds,
   .valueErds = convertibleCompartmentDoorStatusOverrideValueErds,
   .resolvedErd = Erd_ConvertibleCompartmentDoorStatusResolved,
   .numberOfOverrideRequests = NUM_ELEMENTS(convertibleCompartmentDoorStatusOverrideRequestErds)
};

static const Erd_t bottomFreezerDrawerStatusOverrideRequestErds[] = {
   Erd_BottomFreezerDrawerStatus_SabbathOverrideRequest,
   Erd_BottomFreezerDrawerStatus_TestingOverrideRequest
};

static const Erd_t bottomFreezerDrawerStatusOverrideValueErds[] = {
   Erd_BottomFreezerDrawerStatus,
   Erd_BottomFreezerDrawerStatus_SabbathOverrideValue,
   Erd_BottomFreezerDrawerStatus_TestingOverrideValue
};

static const OverrideArbiterConfiguration_t bottomFreezerDrawerStatusOverrideConfiguration = {
   .overrideRequestErds = bottomFreezerDrawerStatusOverrideRequestErds,
   .valueErds = bottomFreezerDrawerStatusOverrideValueErds,
   .resolvedErd = Erd_BottomFreezerDrawerStatusResolved,
   .numberOfOverrideRequests = NUM_ELEMENTS(bottomFreezerDrawerStatusOverrideRequestErds)
};

void FourDoorDoorPlugin_Init(
   FourDoorDoorPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   OverrideArbiter_Init(
      &instance->_private.leftSideFreshFoodDoorOverrideArbiter,
      DataModel_AsDataSource(dataModel),
      &leftSideFreshFoodDoorStatusOverrideConfiguration);

   OverrideArbiter_Init(
      &instance->_private.rightSideFreshFoodDoorOverrideArbiter,
      DataModel_AsDataSource(dataModel),
      &rightSideFreshFoodDoorStatusOverrideConfiguration);

   OverrideArbiter_Init(
      &instance->_private.convertibleCompartmentDoorOverrideArbiter,
      DataModel_AsDataSource(dataModel),
      &convertibleCompartmentDoorStatusOverrideConfiguration);

   OverrideArbiter_Init(
      &instance->_private.bottomFreezerDrawerOverrideArbiter,
      DataModel_AsDataSource(dataModel),
      &bottomFreezerDrawerStatusOverrideConfiguration);

   SabbathInhibitDoors_Init(
      &instance->_private.sabbathInhibitDoors,
      dataModel,
      &sabbathInhibitDoorsConfig);

   AllFreshFoodDoorStatus_Init(
      &instance->_private.allFreshFoodDoorStatus,
      dataModel,
      &allFreshFoodDoorStatusConfiguration);
}
