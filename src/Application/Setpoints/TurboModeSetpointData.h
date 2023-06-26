/*!
 * @file
 * @brief turbo mode setpoint data
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TURBOMODESETPOINTDATA_H
#define TURBOMODESETPOINTDATA_H

#include "TemperatureDegFx100.h"

typedef struct
{
   TemperatureDegFx100_t turboModeSetpointInDegFx100;
   uint16_t turboModeOnTimeInMinutes;
} TurboModeData_t;

typedef struct
{
   const TurboModeData_t *freshFoodTurboCoolSetpointData;
   const TurboModeData_t *freezerTurboFreezeSetpointData;
} TurboModeSetpointData_t;

#endif
