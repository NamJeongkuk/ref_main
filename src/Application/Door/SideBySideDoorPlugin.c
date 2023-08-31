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

static const SabbathDoorResolvedPair_t doorResolvedPairs[] = {
   { .doorStatusErd = Erd_LeftSideFreezerDoorStatus, .doorStatusResolvedErd = Erd_LeftSideFreezerDoorStatusResolved },
   { .doorStatusErd = Erd_RightSideFreshFoodDoorStatus, .doorStatusResolvedErd = Erd_RightSideFreshFoodDoorStatusResolved },
};

static const SabbathInhibitDoorsConfiguration_t sabbathInhibitDoorsConfig = {
   .doorResolvedPairErdList = doorResolvedPairs,
   .numberOfPairs = NUM_ELEMENTS(doorResolvedPairs),
   .sabbathModeErd = Erd_SabbathModeEnable,
   .enhancedSabbathModeErd = Erd_EnhancedSabbathModeEnable,
   .sabbathGpioErd = Erd_Gpio_SABBATH
};

void SideBySideDoorPlugin_Init(SideBySideDoorPlugin_t *instance, I_DataModel_t *dataModel)
{
   SabbathInhibitDoors_Init(
      &instance->_private.sabbathInhibitDoors,
      dataModel,
      &sabbathInhibitDoorsConfig);

   AllFreshFoodDoorStatus_Init(
      &instance->_private.allFreshFoodDoorStatus,
      dataModel,
      &allFreshFoodDoorStatusConfiguration);
}
