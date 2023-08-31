/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef LIGHTINGDATA_H
#define LIGHTINGDATA_H

#include "DoorLightingData.h"
#include "UserAllowableInteriorLightingData.h"

typedef struct
{
   PercentageDutyCycle_t maximumLightDutyCyclePercentage;
   uint8_t maximumCompartmentLightOnTimeInMinutes;
   const UserAllowableInteriorLightingData_t *userAllowableInteriorLightingData;
   const DoorLightingData_t *freshFoodBackWallDoorLightingData;
   const DoorLightingData_t *freshFoodTopAndSideDoorLightingData;
   const DoorLightingData_t *freezerBackWallDoorLightingData;
   const DoorLightingData_t *freezerTopAndSideDoorLightingData;
} LightingData_t;

#endif
