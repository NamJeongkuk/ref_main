/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SingleDoorFreezerDoorPlugin.h"
#include "SystemErds.h"

static const SabbathDoorOverridePair_t sabbathOverrideErdPairs[] = {
   {
      .doorStatusOverrideRequestErd = Erd_LeftSideFreezerDoorStatus_SabbathOverrideRequest,
      .doorStatusOverrideValueErd = Erd_LeftSideFreezerDoorStatus_SabbathOverrideValue,
   }
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

void SingleDoorFreezerDoorPlugin_Init(
   SingleDoorFreezerDoorPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   OverrideArbiter_Init(
      &instance->_private.leftSideFreezerDoorOverrideArbiter,
      DataModel_AsDataSource(dataModel),
      &leftSideFreezerDoorStatusOverrideConfiguration);

   SabbathInhibitDoors_Init(
      &instance->_private.sabbathInhibitDoors,
      dataModel,
      &sabbathInhibitDoorsConfig);
}
