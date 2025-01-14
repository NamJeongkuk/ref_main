/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FourDoorDoorPlugin.h"
#include "SystemErds.h"

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
      .doorStatusOverrideRequestErd = Erd_ConvertibleCompartmentDoorStatus_SabbathOverrideRequest,
      .doorStatusOverrideValueErd = Erd_ConvertibleCompartmentDoorStatus_SabbathOverrideValue,
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
   Erd_ConvertibleCompartmentDoorStatus_SabbathOverrideRequest,
   Erd_ConvertibleCompartmentDoorStatus_TestingOverrideRequest
};

static const Erd_t convertibleCompartmentDoorStatusOverrideValueErds[] = {
   Erd_ConvertibleCompartmentDoorStatus,
   Erd_ConvertibleCompartmentDoorStatus_SabbathOverrideValue,
   Erd_ConvertibleCompartmentDoorStatus_TestingOverrideValue
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

static ConvertibleCompartmentDoorResolverConfiguration_t convertibleCompartmentDoorStateResolverConfiguration = {
   .convertibleCompartmentDoorIsOpenErd = Erd_ConvertibleCompartmentDoorStatusResolved,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .convertibleCompartmentAsFreshFoodDoorIsOpenErd = Erd_ConvertibleCompartmentAsFreshFoodDoorStatus,
   .convertibleCompartmentAsFreezerDoorIsOpenErd = Erd_ConvertibleCompartmentAsFreezerDoorStatus
};

static const Erd_t allFreshFoodDoorsList[] = {
   Erd_RightSideFreshFoodDoorStatusResolved,
   Erd_LeftSideFreshFoodDoorStatusResolved
};

static const Erd_t aFreshFoodDoorIsOpenList[] = {
   Erd_AFreshFoodDoorIsOpen
};

static const ErdLogicServiceConfigurationEntry_t freshFoodDoorsErdLogicServiceEntries[] = {
   { ErdLogicServiceOperator_Or, { allFreshFoodDoorsList, NUM_ELEMENTS(allFreshFoodDoorsList) }, Erd_AFreshFoodDoorIsOpen },
   { ErdLogicServiceOperator_Not, { aFreshFoodDoorIsOpenList, NUM_ELEMENTS(aFreshFoodDoorIsOpenList) }, Erd_AllFreshFoodDoorsAreClosed },
};

static const ErdLogicServiceConfiguration_t freshFoodDoorsErdLogicServiceConfig = {
   freshFoodDoorsErdLogicServiceEntries,
   NUM_ELEMENTS(freshFoodDoorsErdLogicServiceEntries)
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

   ErdLogicService_Init(
      &instance->_private.freshFoodDoorsErdLogicService,
      &freshFoodDoorsErdLogicServiceConfig,
      DataModel_AsDataSource(dataModel));

   ConvertibleCompartmentDoorStateResolver_Init(
      &instance->_private.convertibleCompartmentDoorStateResolver,
      dataModel,
      &convertibleCompartmentDoorStateResolverConfiguration);
}
