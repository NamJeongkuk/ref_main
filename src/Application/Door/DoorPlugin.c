/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DoorPlugin.h"
#include "SystemErds.h"
#include "AllFreshFoodDoorStatus.h"

static struct
{
   AllFreshFoodDoorStatus_t allFreshFoodDoorStatus;
} instance;

static const AllFreshFoodDoorStatusConfiguration_t allFreshFoodDoorStatusConfiguration = {
   .rightFreshDoorIsOpenErd = Erd_RightHandFreshFoodDoorIsOpen,
   .leftFreshDoorIsOpenErd = Erd_LeftHandFreshFoodDoorIsOpen,
   .allFreshFoodDoorsAreClosedErd = Erd_AllFreshFoodDoorsAreClosed
};

void DoorPlugin_Init(I_DataModel_t *dataModel)
{
   AllFreshFoodDoorStatus_Init(&instance.allFreshFoodDoorStatus,
      dataModel,
      &allFreshFoodDoorStatusConfiguration);
}
